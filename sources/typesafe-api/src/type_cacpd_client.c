/**
   @file type_cacpd_client.c
  assistent functions for type-safe api
  collection of functions used by all functions
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include <glib-2.0/glib.h>

#include <dlog.h>

#include "type_cacpd_client.h"
#include "parameter_types.h"

#define DLOG_CONTEXT "TSAPI"

struct gsp_tsapi_session_s {
	cacpdclient_connection_t *connection;
	char *session_id;
	int disconnect_on_idle;
};

cacpdclient_connection_t *gsp_tsapi_get_connection(gsp_tsapi_session_t *sess)
{
	return sess->connection;
}

/*
  replaces the cacpd error codes with those of this client
*/
static typesafe_err adjust_error(cacpdclient_transaction *trans)
{
    switch(cacpdclient_transaction_get_result_code(trans)){
    case CACP_RESULT_OK:
        return TS_OK;
        break;

    case CACP_RESULT_SANITY_CHECK_FAILED:
        return SANITY_FAIL;

    case CACP_RESULT_ERR:
        break;

    case CACP_RESULT_MALFORMED_REQUEST:
        return BAD_COMMAND;
        break;

    default:
        break;
    }
    return UNKNOWN_ERR;
}

/**
 * Ensures that this TSAPI session is disconnected from CACPD.
 */
static void disconnect_from_cacpd(gsp_tsapi_session_t *sess)
{
    if(!sess->connection){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Asked to disconnect, but not connected.");
	return;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Disconnecting from CACPD.");
    cacpdclient_disconnect(sess->connection);
    sess->connection = NULL;
}

/**
 * Ensures that this TSAPI session is connected to CACPD.
 *
 * @return 1 if connected; 0 if a connection could not be established.
 */
static int connect_to_cacpd(gsp_tsapi_session_t *sess)
{
    if (sess->connection){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Asked to connect, but already connected.");
        return 1;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Establishing connection to CACPD.");
    sess->connection = cacpdclient_connect();
    if (!sess->connection)
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to establish connection to CACPD.");

    return (sess->connection != NULL);
}

/**
 * Make the current TSAPI session "ready for action".
 */
typesafe_err tsapi_make_session_ready(gsp_tsapi_session_t *sess)
{
    cacpdclient_transaction *trans;

    typesafe_err result;

    if (!sess->disconnect_on_idle)
        return TS_OK;

    if (!connect_to_cacpd(sess))
	return CONNECT_FAIL;

    if (!sess->session_id)
	return UNKNOWN_ERR;

    trans = cacpdclient_resume(sess->connection, sess->session_id);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to send RESUME request.");
        return CONNECT_FAIL;
    }

    if (cacpdclient_transaction_get_result_code(trans) != CACP_RESULT_OK){
        disconnect_from_cacpd(sess);
    }
    result = adjust_error(trans);
    cacpdclient_destroy_transaction(trans);
    return result;
}

typesafe_err tsapi_make_session_idle(gsp_tsapi_session_t *sess)
{
    if (sess->disconnect_on_idle)
        cacpdclient_disconnect(gsp_tsapi_get_connection(sess));

    return TS_OK;
}

static int tsapi_login(gsp_tsapi_session_t *sess, const char *user, const char*passwd)
{
    enum cacpdclient_transaction_result_code err_code;
    cacpdclient_transaction *trans;
    int result = TS_OK;

    if (!connect_to_cacpd(sess))
	return CONNECT_FAIL;

    trans = cacpdclient_login(sess->connection, user, passwd);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "CACPD sent no response to LOGIN.");
        return CONNECT_FAIL;
    }

    err_code = cacpdclient_transaction_get_result_code(trans);

    if (err_code == CACP_RESULT_OK){
	sess->session_id = cacpdclient_connection_get_sessionid(sess->connection);
        dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Connected with CACPD session ID %s.", sess->session_id);
    } else {
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to log in to CACPD.");
	result = LOGIN_FAIL;
    }
    cacpdclient_destroy_transaction(trans);

    tsapi_make_session_idle(sess);

    return result;
}

int gsp_tsapi_init_with_username_password(gsp_tsapi_session_t **sess, const char *username, const char *password)
{
    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Initializing TSAPI session with username=\"%s\".", username);
    *sess = (gsp_tsapi_session_t *)malloc(sizeof(gsp_tsapi_session_t));
    memset(*sess, 0, sizeof(gsp_tsapi_session_t));
    int result = tsapi_login(*sess, username, password);
    if (result != TS_OK){
	free(*sess);
	*sess = NULL;
    }
    return result;
}	

#ifdef ENABLE_GSPAAA
int gsp_tsapi_init_with_gspaaa_session(gsp_tsapi_session_t **sess, struct sid_data *sdata)
{
    char *sid_str = gsp_auth_session_get_sid(sdata);
    if (NULL == sid_str)
        return UNKNOWN_ERR;

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Initializing TSAPI session with GSP AAA session id \"%s\".", sid_str);

    int result = gsp_tsapi_init_with_username_password(sess, ".gspaaa", sid_str);
    free(sid_str);
    return result;
}
#endif

static int tsapi_logout(gsp_tsapi_session_t *sess)
{
    cacpdclient_transaction *trans;

    if (tsapi_make_session_ready(sess) != TS_OK){
        return LOGIN_FAIL;
    }

    trans = cacpdclient_logout(sess->connection);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to send LOGOUT request to CACPD.");
        return CONNECT_FAIL;
    }

    cacpdclient_destroy_transaction(trans);
    tsapi_make_session_idle(sess);

    return TS_OK;
}

int gsp_tsapi_destroy(gsp_tsapi_session_t *sess){
    int result;

    if (!sess){
        dlog_printf(DLOG_DEBUG, DLOG_CONTEXT, "Asked to destroy a NULL TSAPI session.");
	return TS_OK;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Logging out and terminating TSAPI session.");

    result = tsapi_logout(sess);

    if (sess->connection){
	disconnect_from_cacpd(sess);
    }

    if (sess->session_id){
	free(sess->session_id);
	sess->session_id = NULL;
    }
    free(sess);
    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "TSAPI session destroyed.");

    return result;
}

int gsp_tsapi_start_queue()
{
	int result = create_tsapi_subque();
	if(result)
		return 0;
	else
		return 1;
}

void gsp_tsapi_kill_queue()
{
	cacpdclient_clear_tsapi_submitqueue();
}

char *gsp_tsapi_submit_queue(gsp_tsapi_session_t *sess)
{
	char *result;
	result = cacpdclient_tsapi_submit(sess->connection);
	return result;
}

int gsp_tsapi_check_dirty_session(gsp_tsapi_session_t *sess)
{
    cacpdclient_transaction *trans;
	char *response;
    int result;

    if (tsapi_make_session_ready(sess) != TS_OK){
        return LOGIN_FAIL;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Checking for dirty TSAPI session.");
    trans = cacpdclient_send_dirty(sess->connection);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to send DIRTY request to CACPD.");
		tsapi_make_session_idle(sess);
        return CONNECT_FAIL;
    }

	if (!(response = cacpdclient_transaction_get_value(trans))) {
		dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "An error was encountered processing DIRTY request.");
		result = UNKNOWN_ERR;
	} else {
			result = atoi(response);
	}

    cacpdclient_destroy_transaction(trans);
    tsapi_make_session_idle(sess);
    return result;
}

int gsp_tsapi_revert(gsp_tsapi_session_t *sess)
{
    cacpdclient_transaction *trans;
    int ret_err;

    if (tsapi_make_session_ready(sess) != TS_OK){
        return LOGIN_FAIL;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Reverting changes in TSAPI session.");
    trans = cacpdclient_send_revert(sess->connection);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to send REVERT request to CACPD.");
        return CONNECT_FAIL;
    }

    ret_err = adjust_error(trans);

    cacpdclient_destroy_transaction(trans);
    tsapi_make_session_idle(sess);

    return ret_err;
}

int gsp_tsapi_commit(gsp_tsapi_session_t *sess)
{
    cacpdclient_transaction *trans;
    int ret_err;

    if (tsapi_make_session_ready(sess) != TS_OK){
        return LOGIN_FAIL;
    }

    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Committing changes in TSAPI session.");
    trans = cacpdclient_send_commit(sess->connection);
    if (!trans){
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "Unable to send COMMIT request to CACPD.");
        return CONNECT_FAIL;
    }

    ret_err = adjust_error(trans);

    cacpdclient_destroy_transaction(trans);
    tsapi_make_session_idle(sess);

    return ret_err;
}

int gsp_tsapi_init_with_cid(gsp_tsapi_session_t **sess, const char *cid, int idle)
{
    dlog_printf(DLOG_INFO, DLOG_CONTEXT, "Initializing TSAPI session with cid=\"%s\" and idle=%d.", cid, idle);
    gsp_tsapi_session_t *session = (gsp_tsapi_session_t *)malloc(sizeof(gsp_tsapi_session_t));
	if (!session)
		return UNKNOWN_ERR;
    memset(session, 0, sizeof(gsp_tsapi_session_t));
    session->connection = NULL;
    session->session_id = strdup(cid);
    session->disconnect_on_idle = 1;
    int result = tsapi_make_session_ready(session);
    if (result != TS_OK){
		free(session);
		session = NULL;
    } else {
		session->disconnect_on_idle = idle;
	}

    *sess = session;

    return result;
}

int idle_gsp_session(gsp_tsapi_session_t *sess)
{
    int result = TS_OK;
    if (sess)
    {
        //<change tsapi mode (disconnect on idle)>
        sess->disconnect_on_idle = 1;
        //<close cacpd connection, keeping cacpd session>
        result = tsapi_make_session_idle(sess);
        free(sess);
    }
    return result;
}
