/**
 * @file cacpdparser.c
 * 			The main interface to the cacpd interface library.
 *
 * @ingroup libcacpdparser
 *
 * @authors
 *          Oliver Kurth <oliver.kurth@avocent.com>
 *          Hugh Nelson  <hugh.nelson@avocent.com>
 */

// includes / defines

#include "config.h"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <dlog.h>

#include "libcacpdparser/transaction.h"
#include "transaction_priv.h"

#include "response_priv.h"
#include "libcacpdparser/urlcoding.h"

#include "libcacpdparser/connection.h"
#include "connection_priv.h"

#include "libcacpdparser/commands.h"
#include "commands_priv.h"

#include "libcacpdparser/error.h"

#include <glib-2.0/glib.h>

/// The maximum length of the path for the socket file.
#define UNIX_PATH_MAX    108
/// Location and filename of the socket file.
#ifndef CACPD_SOCKET_PATH
#define CACPD_SOCKET_PATH       "/dev/cacpd"
#endif

#define DLOG_CONTEXT "cacpclient"

struct cacpdclient_submitqueue *tsapi_sq;
int tsapi_subque = 0;
//
// Socket-related / Internal functions

static cacpdclient_transaction *cacpdclient_send_cmd(cacpdclient_connection_t *conn, const char* cmd, enum command_type type) {
	if (cacpdclient_send_subcommand(conn, NULL, cmd))
		return NULL;

	return connection_create_transaction(conn, type);
}

//
// User Functions
/**
 * Establishes a connection with CACPD.  This function does not log in,
 * or send any other commands to CACPD, it just makes a connection.
 */
__attribute__((warn_unused_result))
cacpdclient_connection_t* cacpdclient_connect()
{
	return open_connection(CACPD_SOCKET_PATH);
}


/**
 * Disconnects from CACPD.
 * The connection structure will be destroyed by this operation.
 * @ingroup blah
 */
void cacpdclient_disconnect(cacpdclient_connection_t *conn)
{
	if(!conn) {
		dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Attempted to close a closed connection.");
		return;
	}

	close_connection(conn);

	destroy_connection(conn);
}

/**
 * Sends a command that does not take any parameters to CACPD.
 *
 * @param cmd A command to send to CACPD.
 */
__attribute__((warn_unused_result))
static
cacpdclient_transaction *cacpdclient_send_none(cacpdclient_connection_t *conn, const char *cmd, enum command_type type)
{
	return cacpdclient_send_cmd(conn, cmd, type);
}

/**
 * Sends a command, and a single variable to CACPD.
 *
 * @param cmd A command to send to CACPD.
 *
 * @param address A variable to be sent to CACPD, usually
 * an address.
 */
__attribute__((warn_unused_result))
static
cacpdclient_transaction *cacpdclient_send_one(cacpdclient_connection_t *conn, const char* cmd, const char* address, enum command_type type)
{
	char* output = (char*)g_strdup_printf(cmd, address);
	cacpdclient_transaction *trans = cacpdclient_send_cmd(conn, output, type);
	free(output);
	return trans;
}

__attribute__((warn_unused_result))
static
cacpdclient_transaction *cacpdclient_send_path_args(cacpdclient_connection_t *conn, const char *cmd,
	const char *address, const char * const * args, enum command_type type)
{
	char *buf = NULL;
	if (args && *args){
		int buf_len = 0;
		const char * const * p;
		int i;
		for (p = args; *p; ++p){
			buf_len++;
			buf_len += 3 * strlen(*p); /* urlencoding can't expand the input more than this */
		}

		buf = malloc(buf_len + 1);

		for (p = args, i = 0; *p && i < buf_len; ++p){
			/* A space between args */
			buf[i++] = ' ';

			char *arg_enc = url_encode(*p);
			int l = strlen(arg_enc);
			if (i + l > buf_len)
				/* Would overflow the buffer. */
				break;

			memcpy(buf + i, arg_enc, l);
			free(arg_enc);

			i += l;
		}
		buf[i] = '\0';
	}
	char *output = (char *)g_strdup_printf("%s %s%s",
		cmd, address, (buf ? buf : ""));
	if (buf)
		free(buf);
	cacpdclient_transaction *trans = cacpdclient_send_cmd(conn, output, type);
	free(output);
	return trans;
}

/**
 * Sends a command, and two variables to CACPD.
 *
 * @param cmd A command to send to CACPD.
 *
 * @param address A variable to be sent to CACPD, usually
 * an address.
 *
 * @param value The second variable.
 */
__attribute__((warn_unused_result))
static
cacpdclient_transaction *cacpdclient_send_two(cacpdclient_connection_t *conn,
	const char* cmd, const char* address, const char* value,
	enum command_type type)
{
	char* value_enc = url_encode(value);
	char* output = (char*)g_strdup_printf(cmd, address, value_enc);
    
	cacpdclient_transaction *trans = cacpdclient_send_cmd(conn, output, type);

	free(output);
	free(value_enc);

	return trans;
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_three(cacpdclient_connection_t *conn,
	const char* cmd, const char* address, const char* value1,
	const char *value2, enum command_type type)
{
	char* value1_enc = url_encode(value1);
	char* value2_enc = url_encode(value2);
	char* output = (char*)g_strdup_printf(cmd, address, value1_enc, value2_enc);

	cacpdclient_transaction *trans = cacpdclient_send_cmd(conn, output, type);

	free(output);
	free(value1_enc);
	free(value2_enc);

	return trans;
}

/**
 * Logs a user into CACPD.  You must check the return to see if
 * the login actually happened.
 *
 * @param username A username.
 *
 * @param password A password.
 *
 * @return A document data structure. Returns null if the 
 *      login didn't happen.
 */
cacpdclient_transaction *cacpdclient_login( cacpdclient_connection_t *conn, const char* username, const char* password ) {
    cacpdclient_transaction *trans = cacpdclient_send_two(conn, "LOGIN %s %s", username, password, CACP_COMMAND_LOGIN);

    if (!trans)
        return NULL;
    
    cacpdclient_command_response *resp = cacpdclient_get_response(trans);
    
    if(!resp) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "No response received from CACPD.");
        return NULL; 
    }
    cacpdclient_transaction_get_result_code(trans);
    
    return trans;
}

cacpdclient_transaction *cacpdclient_login_with_gspaaa_session(cacpdclient_connection_t *conn, struct sid_data *sdata)
{
	char *sid_str = gsp_auth_session_get_sid(sdata);
	if (NULL == sid_str){
		dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to obtain SID text for GSPAAA session.");
		return NULL;
	}

	dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Initializing CACPD session with GSP AAA session id \"%s\".", sid_str);

	cacpdclient_transaction *trans = cacpdclient_login(conn, ".gspaaa", sid_str);
	free(sid_str);

	return trans;
}

/**
 * Logs a user into CACPD.  You must check the return to see if
 * the login actually happened.
 *
 * @param username A username.
 *
 * @param password A password.
 *
 * @return A document data structure. Returns null if the 
 *      login didn't happen.
 */
cacpdclient_transaction *cacpdclient_login_with_gspaaa_sid(
	cacpdclient_connection_t *conn, const char* gspaaa_sid)
{
    return cacpdclient_login(conn, ".gspaaa", gspaaa_sid);
}


/**
 * Logs a user out of cacpd.
 */
cacpdclient_transaction *cacpdclient_logout(cacpdclient_connection_t *conn)
{
	cacpdclient_transaction *trans =
		cacpdclient_send_none(conn, "LOGOUT" , CACP_COMMAND_LOGOUT);

	if (trans) {
		/* Make the "logout" operation synchronous by waiting for the response: */
		/* VOID = */ cacpdclient_transaction_get_result_code(trans);
	}
    
	return trans;
}

/**
 * Resumes a CACPD session.
 *
 * @return A document data structure.
 */
cacpdclient_transaction *cacpdclient_resume(cacpdclient_connection_t *conn,
	const char *sessionid)
{
    cacpdclient_transaction *trans = cacpdclient_send_one(conn, "RESUME %s", sessionid, CACP_COMMAND_RESUME);

    if (!trans)
        return NULL;

    cacpdclient_command_response *resp = cacpdclient_get_response(trans);
    if (resp) {
        if (CACP_RESULT_OK == cacpdclient_transaction_get_result_code(trans)) {
            connection_set_sessionid(conn, sessionid);
        }
    }
	
    return trans;
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_get(cacpdclient_connection_t *conn, const char *address)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"get\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn, "GET %s", address, CACP_COMMAND_GET);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_info(cacpdclient_connection_t *conn, const char *address, const char * const *args)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"info\" request for address \"%s\".", address);
	return cacpdclient_send_path_args(conn, "INFO", address, args, CACP_COMMAND_INFO);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_admin(cacpdclient_connection_t *conn, const char *address, const char * const *args)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"admin\" request for address \"%s\".", address);
	return cacpdclient_send_path_args(conn, "ADMIN", address, args, CACP_COMMAND_ADMIN);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_set(cacpdclient_connection_t *conn, const char *address, const char *value) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"set\" request for address \"%s\", new value=\"%s\".", address, value);
	if(tsapi_subque){
		int subid = cacpdclient_submit_set(tsapi_sq,address,value); 
		cacpdclient_transaction *trans = create_transaction(CACP_COMMAND_SET);
		transaction_set_transmit_parameters(trans,conn,subid);
		transaction_set_transmit_result_code(trans,CACP_RESULT_OK);
		return trans;
	}
	else
		return cacpdclient_send_two(conn, "SET %s %s", address, value, CACP_COMMAND_SET);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_list(cacpdclient_connection_t *conn, const char *address) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"list\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn, "LIST %s", address, CACP_COMMAND_LIST);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mlist(cacpdclient_connection_t *conn, const char *address)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"mlist\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn, "MLIST %s", address, CACP_COMMAND_MLIST);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_listr(cacpdclient_connection_t *conn, const char *address) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"listr\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn,"LISTR %s", address, CACP_COMMAND_LISTR);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mlistr(cacpdclient_connection_t *conn, const char *address) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"mlistr\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn,"MLISTR %s", address, CACP_COMMAND_MLISTR);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_noop(cacpdclient_connection_t *conn) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"noop\" request.");
	return cacpdclient_send_none(conn, "NOOP", CACP_COMMAND_NOOP);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_loginforce(cacpdclient_connection_t *conn, const char *username, const char *password) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"loginforce\" request for user \"%s\".", username);
	return cacpdclient_send_two(conn, "LOGINFORCE %s %s", username, password, CACP_COMMAND_LOGINFORCE);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_access(cacpdclient_connection_t *conn, const char *address, const char *access_type)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"access\" request for address \"%s\", access type=\"%s\".", address, access_type);
	return cacpdclient_send_two(conn, "ACCESS %s %s", address, access_type, CACP_COMMAND_ACCESS);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_maccess(cacpdclient_connection_t *conn, const char *address, const char *access_type)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"maccess\" request for address \"%s\", access type=\"%s\".", address, access_type);
	return cacpdclient_send_two(conn, "MACCESS %s %s", address, access_type, CACP_COMMAND_MACCESS);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_add(cacpdclient_connection_t *conn, const char *address, const char *value) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"add\" request for container address \"%s\", new value=\"%s\".", address, value);
	if(tsapi_subque){
		int subid = cacpdclient_submit_add(tsapi_sq,address,value);
                cacpdclient_transaction *trans = create_transaction(CACP_COMMAND_ADD);
                transaction_set_transmit_parameters(trans,conn,subid);
                transaction_set_transmit_result_code(trans,CACP_RESULT_OK);
                return trans;
	}
	else
		return cacpdclient_send_two(conn, "ADD %s %s", address, value, CACP_COMMAND_ADD);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_del(cacpdclient_connection_t *conn, const char *address, const char *value) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"del\" request for container address \"%s\", value=\"%s\".", address, value);
	if(tsapi_subque){
		int subid = cacpdclient_submit_del(tsapi_sq,address,value);
                cacpdclient_transaction *trans = create_transaction(CACP_COMMAND_DEL);
                transaction_set_transmit_parameters(trans,conn,subid);
                transaction_set_transmit_result_code(trans,CACP_RESULT_OK);
                return trans;
	}
        else
		return cacpdclient_send_two(conn, "DEL %s %s", address, value, CACP_COMMAND_DEL);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_move(cacpdclient_connection_t *conn, const char *parent_address, const char *old_name, const char *new_name) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT,
		"Preparing \"move\" request for container address \"%s\", "
			"old name=\"%s\", new name=\"%s\".",
		parent_address, old_name, new_name);
	if(tsapi_subque){
		int subid = cacpdclient_submit_move(tsapi_sq,parent_address,old_name,new_name);
                cacpdclient_transaction *trans = create_transaction(CACP_COMMAND_MOVE);
                transaction_set_transmit_parameters(trans,conn,subid);
                transaction_set_transmit_result_code(trans,CACP_RESULT_OK);
                return trans;
        }
        else
		return cacpdclient_send_three(conn, "MOVE %s %s %s", parent_address, old_name, new_name, CACP_COMMAND_MOVE);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_current(cacpdclient_connection_t *conn) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"current\" request.");
	return cacpdclient_send_none(conn, "CURRENT", CACP_COMMAND_CURRENT);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_attr(cacpdclient_connection_t *conn, const char *address) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"attr\" request for address \"%s\".", address);
	return cacpdclient_send_one(conn, "ATTR %s", address, CACP_COMMAND_MATTR);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mattr(cacpdclient_connection_t *conn, const char *address)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"mattr\" request.");
	return cacpdclient_send_one(conn, "MATTR %s", address, CACP_COMMAND_MATTR);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_revert(cacpdclient_connection_t *conn) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"revert\" request.");
	return cacpdclient_send_none(conn, "REVERT", CACP_COMMAND_REVERT);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_commit(cacpdclient_connection_t *conn) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"commit\" request.");
	return cacpdclient_send_none(conn, "COMMIT", CACP_COMMAND_COMMIT);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_dirty(cacpdclient_connection_t *conn) 
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"dirty\" request.");
	return cacpdclient_send_none(conn, "DIRTY", CACP_COMMAND_DIRTY);
}

#ifdef ENABLE_EVAL_COMMAND
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_eval(cacpdclient_connection_t *conn, const char *expr)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"eval\" request.");
	return cacpdclient_send_one(conn, "EVAL %s", expr, CACP_COMMAND_EVAL);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_funclist(cacpdclient_connection_t *conn)
{
	dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"eval\" request.");
	return cacpdclient_send_none(conn, "FUNCLIST", CACP_COMMAND_FUNCLIST);
}
#endif /*ENABLE_EVAL_COMMAND */

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_update(cacpdclient_connection_t *conn)
{
    dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Preparing \"update\" request.");
    return cacpdclient_send_none(conn, "UPDATE", CACP_COMMAND_UPDATE);
}

int cacpdclient_update_data_changed(cacpdclient_transaction *trans)
{
    cacpdclient_command_response *resp = cacpdclient_get_response(trans);
    if (!resp)
        return 0;

    if (CACP_RESULT_OK != cacpdclient_transaction_get_result_code(trans))
        return 0;

    char *message = cacpdclient_response_get_message(resp);
    if (!message)
        return 0;

    if (!strcmp("updated", message))
        return 1;

    return 0;
}

struct cacpdclient_submitqueue {
    struct submitqueue_element *head, *tail;
    int next_id;
};

struct submitqueue_element {
    struct submitqueue_element *next;
    enum subcommand_type type;
    int subid;
    char *address;
    char *v1, *v2;
};

static int append_submit(cacpdclient_submitqueue *sq, enum subcommand_type type, const char *address, const char *v1, const char *v2)
{
    struct submitqueue_element *new_entry = (struct submitqueue_element *)malloc(sizeof(struct submitqueue_element));
    new_entry->type = type;
    new_entry->address = strdup(address);
    new_entry->v1 = (v1 ? strdup(v1) : NULL);
    new_entry->v2 = (v2 ? strdup(v2) : NULL);
    new_entry->next = NULL;
    new_entry->subid = sq->next_id++;
    if (sq->tail) {
        sq->tail->next = new_entry;
        sq->tail = new_entry;
    } else {
        sq->head = sq->tail = new_entry;
    }
    dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Returning from append");
    return new_entry->subid;
}

struct cacpdclient_submitqueue *create_submitqueue()
{
	struct cacpdclient_submitqueue *sq = (struct cacpdclient_submitqueue *)malloc(sizeof(struct cacpdclient_submitqueue));
	sq->head = sq->tail = NULL;
	sq->next_id = 1;
	return sq;
}

int create_tsapi_subque()
{
	if((tsapi_sq=create_submitqueue())){
		tsapi_subque = 1 ;
		return 1;
	}
	else
		return 0;	
}

int clear_submitqueue(cacpdclient_submitqueue *sq){
	struct submitqueue_element *temp = (struct submitqueue_element *)malloc(sizeof(struct submitqueue_element));
	while(sq->head){
		temp = sq->head;
		if (temp->address) free(temp->address);
		if (temp->v1) free(temp->v1);
		if (temp->v2) free(temp->v2);
		sq->head = temp->next;
		free(temp);
	}
	sq->head = sq->tail = NULL;
	sq->next_id = 1;
	return 1;
}

void cacpdclient_clear_tsapi_submitqueue(){
	
	if(tsapi_sq)
		clear_submitqueue(tsapi_sq);
	tsapi_subque = 0;
}

int cacpdclient_submit_add(cacpdclient_submitqueue *sq, const char *container_address, const char *new_name)
{
	return append_submit(sq, CACP_SUBCOMMAND_ADD, container_address, new_name, NULL);
}

int cacpdclient_submit_del(cacpdclient_submitqueue *sq, const char *container_address, const char *name)
{
	return append_submit(sq, CACP_SUBCOMMAND_DEL, container_address, name, NULL);
}

int cacpdclient_submit_set(cacpdclient_submitqueue *sq, const char *address, const char *new_value)
{
	return append_submit(sq, CACP_SUBCOMMAND_SET, address, new_value, NULL);
}

int cacpdclient_submit_move(cacpdclient_submitqueue *sq, const char *container_address, const char *old_name, const char *new_name)
{
	return append_submit(sq, CACP_SUBCOMMAND_MOVE, container_address, old_name, new_name);
}

__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_submit(cacpdclient_connection_t *conn, cacpdclient_submitqueue *sq) 
{
    struct submitqueue_element *e;

    for (e = sq->head; e; e = e->next){
        char *subid = NULL, *cmd = NULL;
        char *v1_enc, *v2_enc;
        asprintf(&subid, "%i", e->subid);
        if (!subid) {
            dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in send_submit");
            return NULL;
        }

        v1_enc = url_encode(e->v1);
        switch(e->type) {
        case CACP_SUBCOMMAND_ADD:
            asprintf(&cmd, "ADD %s %s", e->address, v1_enc);
            break;

        case CACP_SUBCOMMAND_DEL:
            asprintf(&cmd, "DEL %s %s", e->address, v1_enc);
            break;

        case CACP_SUBCOMMAND_SET:
            asprintf(&cmd, "SET %s %s", e->address, v1_enc);
            break;

        case CACP_SUBCOMMAND_MOVE:
            v2_enc = url_encode(e->v2);
            asprintf(&cmd, "MOVE %s %s %s", e->address, v1_enc, v2_enc);
            free(v2_enc);
            break;
        }
        free(v1_enc);

        if (!cmd) {
            dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unrecognized subcommand type %i", e->type);
            free(subid);
            return NULL;
        }

        int result = cacpdclient_send_subcommand(conn, subid, cmd);
        free(cmd);
        free(subid);

        if (result)
            /* Communication failed */
            return NULL;
    }
    return cacpdclient_send_none(conn, "SUBMIT", CACP_COMMAND_SUBMIT);
}

enum cacpdclient_transaction_result_code cacpdclient_submit_result_for_subcommand(
    cacpdclient_transaction *trans, int id)
{
    char *id_str = NULL;
    asprintf(&id_str, "%d", id);
    if (!id_str) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in submit_result");
        return CACP_RESULT_NONE;
    }

    GSList *result_lines = cacpdclient_transaction_get_list_for_id(trans, id_str);
    free(id_str);

    if (!result_lines) {
        dlog_printf(DLOG_WARNING, DLOG_CONTEXT, "No result for id %i.", id);
        return CACP_RESULT_NONE;
    }

    if (result_lines && result_lines->next) {
        dlog_printf(DLOG_WARNING, DLOG_CONTEXT, "Invalid response for id %i.", id);
        g_slist_free(result_lines);
        return CACP_RESULT_MALFORMED_RESPONSE;
    }

    /* These can only return OK or ERR */
    enum cacpdclient_transaction_result_code result;
    if (!strcmp(result_lines->data, "OK"))
        result = CACP_RESULT_OK;
    else if (!strcmp(result_lines->data, "ERR"))
        result = CACP_RESULT_ERR;
    else
        result = CACP_RESULT_MALFORMED_RESPONSE;
    
    g_slist_free(result_lines);
    return result;
}

static cacpdclient_verbose_error_data *error_for_subcommand(
    cacpdclient_transaction *trans, const char const *subid)
{
    char *id_str = NULL;
    if (subid)
        asprintf(&id_str, "%s.error", subid);
    else
        id_str = "error";

    if (!id_str) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in submit_error");
        return NULL;
    }

    char *error_key = cacpdclient_transaction_get_value_for_id(trans, id_str);
    if (subid)
        free(id_str);

    id_str = NULL;

    if (!error_key)
        return NULL;

    if (subid)
        asprintf(&id_str, "%s.error.arg", subid);
    else
        id_str = "error.arg";

    if (!id_str) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in submit_error");
        return NULL;
    }

    GSList *error_args = cacpdclient_transaction_get_list_for_id(trans, id_str);
    if (subid)
        free(id_str);

    cacpdclient_verbose_error_data *err = cacpdclient_create_verbose_error_data(error_key, error_args);

    free(error_key);
    g_slist_free(error_args);

    return err;
}

cacpdclient_verbose_error_data *cacpdclient_submit_error_for_subcommand(
    cacpdclient_transaction *trans, int id)
{
    char *subid = NULL;
    asprintf(&subid, "%i", id);
    if (subid)
        return error_for_subcommand(trans, subid);

    return NULL;
}

cacpdclient_verbose_error_data *cacpdclient_transaction_error(
    cacpdclient_transaction *trans)
{
    return error_for_subcommand(trans, NULL);
}

GSList *cacpdclient_submit_error_for_subcommand_as_list(
    cacpdclient_transaction *trans, int id)
{
    /*
     * Please don't use this function.  Seriously.
     * It's only here so we can get uicontrold done.  New code should use
     * the verbose error data functions.
     */
    char *id_str = NULL;
    asprintf(&id_str, "%d.error", id);
    if (!id_str) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in submit_result");
        return NULL;
    }

    GSList *result_lines = cacpdclient_transaction_get_list_for_id_starting_with(trans, id_str);
    free(id_str);

    return result_lines;
}

int cacpdclient_attr_operations(
    cacpdclient_transaction *trans)
{
    char *ops = cacpdclient_transaction_get_value_for_id(trans, "ops");
    int result = 0;
    char *o;

    for (o = ops; *o; o++) {
        switch (*o) {
        case 'a': result |= CACPCLIENT_NODE_ADD; break;
        case 'd': result |= CACPCLIENT_NODE_DEL; break;
        case 'g': result |= CACPCLIENT_NODE_GET; break;
        case 's': result |= CACPCLIENT_NODE_SET; break;
        case 'l': result |= CACPCLIENT_NODE_LIST; break;
        case 'm': result |= CACPCLIENT_NODE_MOVE; break;
        case 'A': result |= CACPCLIENT_NODE_ADMIN; break;
        case 'I': result |= CACPCLIENT_NODE_INFO; break;
        case 'h': result |= CACPCLIENT_NODE_HELP; break;

        default:
            dlog_printf(DLOG_WARNING, DLOG_CONTEXT, "Unknown ATTR.ops '%c'.",
                *o);
        }
    }
    free(ops);

    return result;
}

char *cacpdclient_tsapi_submit(cacpdclient_connection_t *conn)
{
	struct submitqueue_element *e;
	cacpdclient_transaction *trans = NULL;
	trans = cacpdclient_send_submit(conn, tsapi_sq);
	if ( NULL == trans ){
        	return strdup("CORE_TRANS_FAIL");
    	}
	
	for (e = tsapi_sq->head; e; e = e->next){
        	char *suberr = NULL;

		if (CACP_RESULT_OK == cacpdclient_submit_result_for_subcommand(trans, e->subid)) {
        	        /* This subcommand was successful, don't need to do anything */
	                continue;
        	}
		else{
			char *id_str = NULL;
    			if (e->subid)
        			asprintf(&id_str, "%i.error", e->subid);

			suberr = cacpdclient_transaction_get_value_for_id(trans,id_str);
//			cacpdclient_verbose_error_data *err = cacpdclient_submit_error_for_subcommand(trans, e->subid);
			if(suberr){
				char *new_id_str = NULL;
				clear_submitqueue(tsapi_sq);
	                        tsapi_subque = 0;
				dlog_printf(DLOG_INFO, DLOG_CONTEXT, "idstr %s suberr : %s",id_str,suberr);
				asprintf(&new_id_str, "%i.%s",e->subid,suberr);
	//			suberr = cacpdclient_verbose_error_description(err);
	//			cacpdclient_destroy_verbose_error_data(err);

				cacpdclient_destroy_transaction(trans);
				if(suberr)
					free(suberr);
				return strdup(new_id_str);
			}
			else{
				asprintf(&suberr, "%i ERR", e->subid);
				clear_submitqueue(tsapi_sq);
		        	tsapi_subque = 0;
				cacpdclient_destroy_transaction(trans);
				dlog_printf(DLOG_INFO, DLOG_CONTEXT, "suberr : %s",suberr);
				return strdup(suberr);
			}
		}
	}	
    	clear_submitqueue(tsapi_sq);
	tsapi_subque = 0;
	cacpdclient_destroy_transaction(trans);
    	return NULL;
}	

