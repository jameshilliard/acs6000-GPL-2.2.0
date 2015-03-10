#ifndef CONNECTION_PRIV_H_INCLUDED
#define CONNECTION_PRIV_H_INCLUDED

#include "libcacpdparser/connection.h"
#include "commands_priv.h"

int cacpdclient_send_subcommand(cacpdclient_connection_t *conn, const char *subid, const char *cmd);
cacpdclient_connection_t* open_connection(const char *name);
void close_connection(cacpdclient_connection_t *conn);
void destroy_connection(cacpdclient_connection_t *conn);

cacpdclient_transaction *connection_create_transaction(cacpdclient_connection_t *conn, enum command_type type);
void connection_set_sessionid(cacpdclient_connection_t *conn, const char *sessionid);

#endif /* CONNECTION_PRIV_H_INCLUDED */
