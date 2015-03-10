/**
 * @file response_priv.h Header for response.c
 */

#ifndef CACPDPARSER_RESPONSE_PRIV_H_INCLUDED
#define CACPDPARSER_RESPONSE_PRIV_H_INCLUDED

#include <libcacpdparser/transaction.h>
#include <libcacpdparser/connection.h>
/**
 * Opaque handle representing the server's raw response to a CACP command.
 */
typedef struct cacpdclient_command_response cacpdclient_command_response;

cacpdclient_command_response *create_command_response();
cacpdclient_command_response *create_command_response_ok();
void handle_final(cacpdclient_command_response *resp, gchar* msg);
void handle_cont( cacpdclient_command_response *el, gchar* header, gchar* content );

enum cacpdclient_transaction_result_code cacpdclient_response_get_result_code(cacpdclient_command_response *resp);
char *cacpdclient_response_get_message(cacpdclient_command_response *resp);
const char const *cacpdclient_result_code_description(enum cacpdclient_transaction_result_code result_code);
char *cacpdclient_response_get_result_description(cacpdclient_command_response *resp);

void cacpdclient_destroy_command_response(cacpdclient_command_response *resp);
char* cacpdclient_login_response_extract_session(cacpdclient_command_response *resp);
cacpdclient_command_response *cacpdclient_fetch_next_response(cacpdclient_connection_t *conn);
void displayResponse(cacpdclient_command_response *resp);

gchar *cacpdclient_response_get_value(cacpdclient_command_response *resp);
GSList *cacpdclient_response_get_list_for_id(cacpdclient_command_response *resp, const char const *id);
GSList *cacpdclient_response_get_list_for_id_starting_with(cacpdclient_command_response *resp, const char const *id);
GSList *cacpdclient_response_get_list(cacpdclient_command_response *resp);
GSList *cacpdclient_response_get_choices(cacpdclient_command_response *resp);

#endif

