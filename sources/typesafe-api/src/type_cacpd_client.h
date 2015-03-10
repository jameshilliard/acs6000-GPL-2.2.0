/**
   @file type_cacpd_client.h
   connection to cacpd for the type_safe_api
*/

#ifndef TYPE_CACPD_CLIENT_H
#define TYPE_CACPD_CLIENT_H

#include <gsp_auth.h>

#include <libcacpdparser/cacpdparser.h>

/**
 * List of error codes which may be returned by the Typesafe API and Pathsafe API.
 */
typedef enum{
    TS_CONVERSION_FAILURE = -16, /**< The retrieved value could not be converted to a type-safe value. */
    COMMAND_FAIL = -15,
    BAD_COMMAND = -5 , /**< B, BAD -- CACPD indicated that it received an invalid command. */
    LOGIN_FAIL = -4, /**< E, ERR -- CACPD indicated that the session was already logged in. */
    SANITY_FAIL = -3, /**< E, ERR -- CACPD indicated that the sanity check failed */
    CONNECT_FAIL = -2, /**< not yet inplemented in cacpd_parser */
    UNKNOWN_ERR = -1, /**< An unknown error occurred. */
    TS_OK = 0 /**< The operation completed successfully. */
} typesafe_err;

/**
 * A Typesafe API session handle.  The same session handle is also valid for
 * use with the path-safe API.
 */
typedef struct gsp_tsapi_session_s gsp_tsapi_session_t;


/**
 * Initialize a Typesafe API session using a username and password.
 * @param [out] sess The created Typesafe API session.
 * @param [in] username The username for authentication.
 * @param [in] password The password for authentication.
 * @return One of TS_OK, CONNECT_FAIL or LOGIN_FAIL.
 */
int gsp_tsapi_init_with_username_password(gsp_tsapi_session_t **sess, const char *username, const char *password);

/**
 * Initialize a Typesafe API session using a GSP AAA session.
 * @param [out] sess The created Typesafe API session.
 * @param [in] sdata The GSP Core session data structure.
 * @return One of TS_OK, CONNECT_FAIL or LOGIN_FAIL.
 */
int gsp_tsapi_init_with_gspaaa_session(gsp_tsapi_session_t **sess, struct sid_data *sdata);

/**
 * Obtain the cacpdclient connection currently in use by this Typesafe API
 * session.
 * @param [in] sess The Typesafe API session.
 * @return The cacpdclient connection currently in use by this Typesafe API
 *	session.
 */
cacpdclient_connection_t *gsp_tsapi_get_connection(gsp_tsapi_session_t *sess);

/**
 * Destroy a Typesafe API session.
 * @param [in] sess The Typesafe API session to terminate.
 * @return LOGIN_FAIL if unable to connect to CACPD to properly log out of session.
 *	TS_OK otherwise.
 */
int gsp_tsapi_destroy(gsp_tsapi_session_t *sess);

/**
 * Check if current Typesafe API session has changes respect to backend.
 * @param [in] sess The Typesafe API session.
 * @return LOGIN_FAIL if unable to connect to CACPD to execute the command.
 *	CONNECT_FAIL if unable to send command, other errors if transaction is not OK.
 *	0 if session didn't changed
 *  1 if session changed (is dirty)
 */
int gsp_tsapi_check_dirty_session(gsp_tsapi_session_t *sess);

/**
 * Revert current Typesafe API session to previous backend configuration.
 * @param [in] sess The Typesafe API session.
 * @return LOGIN_FAIL if unable to connect to CACPD to execute the command.
 *	CONNECT_FAIL if unable to send command
 *  other errors if transaction is not OK.
 */
int gsp_tsapi_revert(gsp_tsapi_session_t *sess);

/**
 * Commit the current Typesafe API session's writes in the backend.
 * Before this function returns, the backend's data will have been written
 * out to storage, and any post-commit actions will have been performed
 * (i.e. signalling processes).  Note that this does not guarantee that any
 * dispatched signals have actually been handled by their intended recipients.
 * @param [in] sess The Typesafe API session.
 * @return LOGIN_FAIL if unable to connect to CACPD to execute the command.
 *	CONNECT_FAIL if unable to send command
 *  other errors if transaction is not OK.
 */
int gsp_tsapi_commit(gsp_tsapi_session_t *sess);

/**
 * Initialize a Typesafe API session using cid and idle flag.
 * @param [out] sess The created Typesafe API session.
 * @param [in] cid from aaa.
 * @param [in] idle. The state of disconnect_on_idle.
 * @return One of TS_OK, CONNECT_FAIL or LOGIN_FAIL.
 */
int gsp_tsapi_init_with_cid(gsp_tsapi_session_t **sess, const char *cid, int idle);

/**
 * Make a Typesafe API session idle. Free session structure.
 * @param [in/out] sess The created Typesafe API session.
 */
int idle_gsp_session(gsp_tsapi_session_t *sess);

/**
 * Call libcacpdparser to create a submit queue.
 * @param [in] void.
 * @param [out] 0 if success 1 if failed.
 */
int gsp_tsapi_start_queue(void);

/**
 * Call libcacpdparser to submit queue.
 * @param [in] The created Typesafe API session.
 * @param [out] Error string with sub id; NULL when successful.
 */
char *gsp_tsapi_submit_queue(gsp_tsapi_session_t *sess);

/**
 * Call libcacpdparser to delete a submit queue.
 * @param [in] void.
 * @param [out] void
 */
void gsp_tsapi_kill_queue(void);

#endif
