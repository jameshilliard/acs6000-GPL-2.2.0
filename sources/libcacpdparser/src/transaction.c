#include "libcacpdparser/transaction.h"
#include "response_priv.h"
#include "libcacpdparser/connection.h"
#include "connection_priv.h"
#include "commands_priv.h"

#include <string.h>
#include <stdlib.h>

#include <dlog.h>
#include <glib-2.0/glib.h>

#define DLOG_CONTEXT "cacpclient"

struct cacpdclient_transaction {
	cacpdclient_connection_t *connection;
	enum command_type type;
	int id;
	cacpdclient_command_response *response;
};

cacpdclient_transaction *create_transaction(enum command_type type)
{
	cacpdclient_transaction *trans = (cacpdclient_transaction *)malloc(sizeof(struct cacpdclient_transaction));
	trans->connection = NULL;
	trans->type = type;
	trans->response = NULL;
	trans->id = 0;

	return trans;
}

void transaction_set_transmit_parameters(cacpdclient_transaction *trans, cacpdclient_connection_t *conn, int id)
{
	if (trans->id)
		dlog_printf(DLOG_ERROR, DLOG_CONTEXT,
			"Setting ID %i into transaction which already has ID %i.",
			id, trans->id);

	trans->connection = conn;
	trans->id = id;
}
void transaction_set_transmit_result_code(cacpdclient_transaction *trans, enum cacpdclient_transaction_result_code result )
{
		trans->response = create_command_response_ok();		
}

cacpdclient_command_response *cacpdclient_get_response(cacpdclient_transaction *trans)
{
	if (!trans->response) {
		/*
		 * FIXME: read until we've got the response, PROPERLY... here we assume
		 * that the client will request the responses in order.
		 */
		trans->response = cacpdclient_fetch_next_response(trans->connection);

		if (((CACP_COMMAND_LOGIN == trans->type) || (CACP_COMMAND_LOGINFORCE == trans->type))) {
			if (CACP_RESULT_OK == cacpdclient_transaction_get_result_code(trans)) {
				char *sid = cacpdclient_login_response_extract_session(trans->response);
				connection_set_sessionid(trans->connection, sid);
				free(sid);
			} else
				connection_set_sessionid(trans->connection, NULL);
		}
	}

	return trans->response;
}

void cacpdclient_destroy_transaction(cacpdclient_transaction *trans)
{
	if (trans->response)
		cacpdclient_destroy_command_response(trans->response);

	free(trans);
}

enum cacpdclient_transaction_result_code cacpdclient_transaction_get_result_code(cacpdclient_transaction *trans)
{
	cacpdclient_get_response(trans);

	if (!trans->response)
		return CACP_RESULT_CONNECTION_FAILED;

	return cacpdclient_response_get_result_code(trans->response);
}

char *cacpdclient_transaction_get_result_description(cacpdclient_transaction *trans)
{
	cacpdclient_get_response(trans);

	if (!trans->response)
		return NULL;

	return cacpdclient_response_get_result_description(trans->response);
}

char *cacpdclient_transaction_get_value(cacpdclient_transaction *trans)
{
	if (CACP_RESULT_OK != cacpdclient_transaction_get_result_code(trans))
		return NULL;

	return cacpdclient_response_get_value(cacpdclient_get_response(trans));
}

/**
 * Check whether we have a reasonable result (i.e. something that we can look at).
 */
static int have_valid_response(cacpdclient_transaction *trans)
{
	switch (cacpdclient_transaction_get_result_code(trans)) {
	case CACP_RESULT_NONE:
	case CACP_RESULT_MALFORMED_RESPONSE:
	case CACP_RESULT_CONNECTION_FAILED:
		return 0;

	default:
		break;
	}

	return 1;
}

GSList *cacpdclient_transaction_get_list(cacpdclient_transaction *trans)
{
	if (!have_valid_response(trans))
		return NULL;

	return cacpdclient_response_get_list(cacpdclient_get_response(trans));
}

GSList *cacpdclient_transaction_get_list_for_id(cacpdclient_transaction *trans, const char const *id)
{
	if (!have_valid_response(trans))
		return NULL;

	return cacpdclient_response_get_list_for_id(cacpdclient_get_response(trans), id);
}

char *cacpdclient_transaction_get_value_for_id(cacpdclient_transaction *trans, const char const *id)
{
    GSList *lines = cacpdclient_transaction_get_list_for_id(trans, id);
    if (!lines)
        return NULL;

    int required_len = 0;
    GSList *p;
    for (p = lines; p; p = p->next)
        required_len += strlen((char *)p->data) + 1;

    char *retval = (char *)malloc(required_len + 1);
    if (!retval) {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Memory allocation error in get_value_for_id");
        g_slist_free(lines);
        return NULL;
    }

    int offset = 0;

    for (p = lines; p; p = p->next) {
        int len = strlen((char *)p->data);
        strncpy(retval + offset, (char *)p->data, len);
        offset += len;

        if (p->next)
            retval[offset++] = '\n';
    }
    retval[offset] = '\0';
    g_slist_free(lines);

    return retval;
}

GSList *cacpdclient_transaction_get_list_for_id_starting_with(cacpdclient_transaction *trans, const char const *id)
{
	if (!have_valid_response(trans))
		return NULL;

	return cacpdclient_response_get_list_for_id_starting_with(cacpdclient_get_response(trans), id);
}

GSList *cacpdclient_transaction_get_choices(cacpdclient_transaction *trans)
{
	if (CACP_RESULT_OK != cacpdclient_transaction_get_result_code(trans))
		return NULL;

	return cacpdclient_response_get_choices(cacpdclient_get_response(trans));
}

