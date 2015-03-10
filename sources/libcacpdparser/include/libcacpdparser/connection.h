#ifndef CACPDPARSER_CONNECTION_H_INCLUDED
#define CACPDPARSER_CONNECTION_H_INCLUDED

#include <libcacpdparser/transaction.h>
/**
 * Opaque handle representing a connection to the server.
 * Create using \cacpdclient_connect, destroy using \cacpdclient_disconnect
 */
typedef struct cacpdclient_connection_s cacpdclient_connection_t;

/**
 * Obtains a copy of the CACP server session ID (also known as the CID, or Core ID)
 * which is currently in use by this connection.  The the CACP server session ID may be
 * used through the \cacpdclient_resume function to reconnect to the CACP server without
 * requiring reauthentication, and persisting the session's state.  This ID
 * becomes invalid if the session times out, or upon LOGOUT.  If you wish to
 * later resume a session, simply disconnect it using \cacpdclient_disconnect.
 *
 * @param [in] conn The connection for which the CACP server session ID is requested.
 * @return A NUL-terminated character string representing the CACP server session ID,
 *      or NULL if there is currently no session active on this connection
 *      (i.e. not yet issued a LOGIN or RESUME).  If the return value is not
 *      NULL, the caller owns it and must free() it when no longer needed.
 */
char *cacpdclient_connection_get_sessionid(cacpdclient_connection_t *conn);

#endif /* CACPDPARSER_CONNECTION_H_INCLUDED */
