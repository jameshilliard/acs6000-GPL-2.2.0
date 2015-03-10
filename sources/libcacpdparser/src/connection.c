#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dlog.h>

#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <signal.h>

#include <glib-2.0/glib.h>

#include "libcacpdparser/connection.h"
#include "connection_priv.h"

#include "libcacpdparser/transaction.h"
#include "transaction_priv.h"

#include "response_priv.h"
#include "libcacpdparser/urlcoding.h"

/// The maximum length of the path for the socket file.
#define UNIX_PATH_MAX    108
/// Location and filename of the socket file.
#ifndef CACPD_SOCKET_PATH
#define CACPD_SOCKET_PATH       "/dev/cacpd"
#endif

#define DLOG_CONTEXT "cacpclient"

/// The maximum buffer size for the input buffer from CACPD.
#define MAX_LINE_LEN 65536

/// Index for the line header.
#define HEADER 0

/// Index for the line content.
#define CONTENT 1

/**
 * Interface to a cacpd socket connection.
 *
 */
struct cacpdclient_connection_s{
    /// The session id.
    char* sessionid;
    /// File structure for the input to the socket.
    FILE* fin;
    /// File structure for the output from the socket.
    FILE* fout;
    /// A single-character connection tag (for debugging)
    char connection_tag;
    /// The id of the last command sent to CACPD.
    int next_id;
    /// A count of the responses in a request.
    int outstanding_transactions;
};

/**
 * The connection tag is "unique" to the connection, and provides a means to easily
 * observe the connection in which a command/response occurs.
 */
char g_next_connection_tag = 'A';


static int connect_unix(const char *path) {
    int s;
    struct sockaddr_un sa;
    
    s = socket(PF_UNIX, SOCK_STREAM, 0);
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path, UNIX_PATH_MAX);
    if(connect(s, (struct sockaddr *)&sa, sizeof(sa)) != 0){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "connect('%s') failed: %s", path, strerror(errno));
        return 0;
    }
    return s;
}

cacpdclient_connection_t* open_connection(const char *name) {
    cacpdclient_connection_t* connection;
    
    connection = (cacpdclient_connection_t* )malloc(sizeof(cacpdclient_connection_t));
    if(connection){
        memset(connection, 0, sizeof(cacpdclient_connection_t));
        
        connection->next_id = 1;
        connection->connection_tag = g_next_connection_tag;

        /* Move along to the next connection tag. */
        if ('z' == g_next_connection_tag)
            g_next_connection_tag = 'A';
        else if ('Z' == g_next_connection_tag)
            g_next_connection_tag = 'a';
        else g_next_connection_tag++;
        
        int fd = connect_unix(name);
        if(fd > 0){
            int fd1 = dup(fd);
            connection->fin = fdopen(fd, "r");
            connection->fout = fdopen(fd1, "w");
        } else {
            free(connection);
            connection = NULL;
        }
    }
    return connection;
}

static void disconnect_connection(cacpdclient_connection_t *conn)
{
	if (!conn->fin)
		return;

	fclose(conn->fin);
	fclose(conn->fout);
	conn->fin = NULL;
	conn->fout = NULL;
}

void close_connection(cacpdclient_connection_t *conn)
{
	/* There should be no outstanding transactions. */
	if (conn->outstanding_transactions) {
		dlog_printf(DLOG_WARNING, DLOG_CONTEXT,
			"Closed a connection with %d transaction response(s) pending.",
			conn->outstanding_transactions);
		
		/* Drain the queue... */
		while (conn->outstanding_transactions)
			cacpdclient_fetch_next_response(conn);
	}
}

/**
 * Obtain a copy of the session ID for this session.
 * @param [in] conn The CACPD session for which the session ID is requested.
 * @return A malloced string containing the session ID.  The caller is
 * 	responsible for freeing this when done.
 */
char *cacpdclient_connection_get_sessionid(cacpdclient_connection_t *conn){
	if(conn->sessionid)
		return strdup(conn->sessionid);

	return NULL;
}

void connection_set_sessionid(cacpdclient_connection_t *conn, const char *sessionid)
{
	if (conn->sessionid)
		free(conn->sessionid);
	conn->sessionid = strdup(sessionid);
}

void destroy_connection(cacpdclient_connection_t *conn)
{
	disconnect_connection(conn);

	if(conn->sessionid)
		free(conn->sessionid);

	free(conn);
}

/**
 * Sends a command or subcommand to the CACP server.
 *
 * @param [in] conn The connection handle to which the command should be sent.
 * @param [in] subid A string subidentifier to send, or NULL to send the final
 *      command.
 * @param [in] cmd The formatted, encoded string of the command.
 * @return Zero on success, nonzero on failure.
 */
int cacpdclient_send_subcommand(cacpdclient_connection_t *conn, const char *subid, const char *cmd)
{
    int result;

    if (!conn || !conn->fout) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Attempted to send while not connected.");
        return 1;
    }
    
    char* output = NULL;

    asprintf(&output, "%c%d%s%s %s\n", conn->connection_tag, conn->next_id,
        (subid ? "." : ""), (subid ? subid : ""), cmd);

    sighandler_t old_handler = signal(SIGPIPE, SIG_IGN); /* Hope no-one else is using it at this instant! */

    result = fputs(output, conn->fout);
    if (result != EOF)
        result = fflush(conn->fout);

    signal(SIGPIPE, old_handler);

    if (result) { 
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT,
            "Unable to send to CACP server (result=%i, error=%i [%s]).  Connection lost.",
            result, errno, strerror(errno));
        disconnect_connection(conn);
	return 1;
    }

// #ifdef SHOW_LINES
    output[strlen(output) - 1] = '\0';
    dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Sent: \"%s\".", output);
// #endif

    free(output);
    return 0;
}

cacpdclient_transaction *connection_create_transaction(cacpdclient_connection_t *conn, enum command_type type)
{
	cacpdclient_transaction *trans = create_transaction(type);
	conn->outstanding_transactions += 1; /* FIXME: add "trans" itself to the "pipeline" */
	transaction_set_transmit_parameters(trans, conn, conn->next_id++);
	return trans;
}


static char *read_buffer(FILE *hCacpd)
{
	int bsize = 256; /* Initial buffer size, expanded by x2 until done. */
	char *buf = malloc(bsize);
	int pos = 0;

	*buf = '\0';

	while(1){
		char *result = fgets(&buf[pos], bsize-pos, hCacpd);
		if (NULL == result){
			if(errno == ERESTART)
				continue;

			dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Error or EOF with nothing read; errno=%i [%s].",
				errno, strerror(errno));
			break;
		}

		pos += strlen(&buf[pos]);

		if (pos && ('\n' == buf[pos-1])){
			buf[pos-1] = '\0';
			dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Received: \"%s\".", buf);
			return buf;
		}

		if (MAX_LINE_LEN == bsize){
			dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Maximum buffer length of %d was exceeded.", MAX_LINE_LEN);
			break;
		}

		/* Not enough buffer... keep expanding it. */
		bsize *= 2;
		if (bsize > MAX_LINE_LEN)
			bsize = MAX_LINE_LEN;

		char *newbuf = realloc(buf, bsize);
		if (!newbuf){
			dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error.");
			break;
		}
		buf = newbuf;
	}

	/* Error... */
	free(buf);
	return NULL;
}

static char *read_decoded_buffer(FILE *hCacpd)
{
	char *buffer = read_buffer(hCacpd);
	if(buffer){
		char *decoded_buffer = url_decode(buffer);
		free(buffer);
		
#ifdef SHOW_LINES
		fprintf( stderr, "<<<: [%s]\n", decoded_buffer );
#endif
		
		return decoded_buffer;
	}
	return NULL;
}

// Create

/**
 * Reads the next response from CACPD.
 *
 * @return The response to the given command.
 */
cacpdclient_command_response *cacpdclient_fetch_next_response(cacpdclient_connection_t *conn)
{
	gchar *buff;

	int parse_done = 0;
	cacpdclient_command_response *el = create_command_response();

	while( !parse_done )
	{
		gchar** frag;
		buff = read_decoded_buffer( conn->fin );

		if( !buff )
		{
		    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Unable to read input.");
		    break;
		}

		frag = g_strsplit( buff, " ", 2 );

		if( strchr( frag[ HEADER ], '.' ) )
			handle_cont( el, frag[ HEADER ], frag[ CONTENT ] );
		else
		{
			parse_done = TRUE;
			handle_final( el, frag[ CONTENT ] );
		}
		g_strfreev(frag);
		free(buff);
	}
	conn->outstanding_transactions--;
	return el;
}

