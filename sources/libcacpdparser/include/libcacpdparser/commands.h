#ifndef CACP_COMMANDS_H_INCLUDED
#define CACP_COMMANDS_H_INCLUDED

#include <gsp_auth.h>

#include <libcacpdparser/connection.h>
#include <libcacpdparser/transaction.h>
#include <libcacpdparser/error.h>

/**
 * Opaque handle representing a set of queued commands that are to be sent
 * using the SUBMIT command.
 */
typedef struct cacpdclient_submitqueue cacpdclient_submitqueue;


/**
 * Node operations
 */
enum cacpdclient_node_operations {
    CACPCLIENT_NODE_ADD = 0x0001,
    CACPCLIENT_NODE_DEL = 0x0002,
    CACPCLIENT_NODE_GET = 0x0004,
    CACPCLIENT_NODE_SET = 0x0008,
    CACPCLIENT_NODE_LIST = 0x0010,
    CACPCLIENT_NODE_MOVE = 0x0020,
    CACPCLIENT_NODE_ADMIN = 0x0040,
    CACPCLIENT_NODE_INFO = 0x0080,
    CACPCLIENT_NODE_HELP = 0x0100,
};

/**
 * Logs into the CACP server with the given username and password.
 *
 * @param [in] conn The connection over which the LOGIN request should be sent.
 * @param [in] username The username for authentication.
 * @param [in] password The password for authentication.
 *
 * @return A CACP transaction representing the "login" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_login(cacpdclient_connection_t *conn, const char *username, const char *password);

/**
 * Logs into the CACP server using the given GSP AAA session for authentication.
 *
 * @param [in] conn The connection over which the LOGIN request should be sent.
 * @param [in] sdata The GSP AAA session data structure.
 *
 * @return A CACP transaction representing the "login" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_login_with_gspaaa_session(cacpdclient_connection_t *conn, struct sid_data *sdata);

/**
 * Sends a loginforce to the CACP server.  This should no longer be necessary... it was
 * previously used when CACPD only supported a single administrator at a time.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] username The username for authentication.
 * @param [in] password The password for authentication.
 *
 * @return A CACP transaction representing the "loginforce" request, or NULL if
 *      a communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_loginforce(cacpdclient_connection_t *conn, const char *username, const char *password);

/**
 * Sends a request to resume an existing session with a CACP server.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] sessionid The CACP server session ID, as previously obtained by
 *      \cacpdclient_connection_get_sessionid().
 * @return A CACP transaction representing the "resume" request, or NULL if a
 *      communication error has occurred.
*/
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_resume(cacpdclient_connection_t *conn, const char *sessionid);


/**
 * Logs you out of the CACP server.
 *
 * @param [in] conn The connection over which the session is to be terminated.
 * @return A CACP transaction representing the logout request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_logout(cacpdclient_connection_t *conn);



/**
 * Sends a request to get the value at a given node in the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the value you wish to retrieve from
 *      the CACP server.  Components of this path must be URL-encoded.
 * @return A CACP transaction representing the "get" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_get(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a request to run the "info:" function at a given node in the
 * management tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @param [in] args An optional NULL-terminated list of arguments to the
 *      management request.  If no arguments are required, simply pass NULL in
 *      the parameter.
 * @return A CACP transaction representing the "info" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_info(cacpdclient_connection_t *conn, const char *address, const char * const *args);

/**
 * Sends a request to run the "admin:" function at the given node in the
 * management tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @param [in] args An optional NULL-terminated list of arguments to the
 *      management request.  If no arguments are required, simply pass NULL in
 *      the parameter.
 * @return A CACP transaction representing the "admin" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_admin(cacpdclient_connection_t *conn, const char *address, const char * const *args);


/**
 * Sets a new value into a given node in the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @param [in] value The new value for the node.
 * @return A CACP transaction representing the "set" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_set(cacpdclient_connection_t *conn, const char *address, const char *value);

/**
 * Sends a request for a list of the immediate child nodes of a node in the
 * parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @return A CACP transaction representing the "list" request, or NULL if a
 *      communication error has occurred.
*/
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_list(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a request for a list of the immediate child nodes of a node in the
 * management tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @return A CACP transaction representing the "mlist" request, or NULL if a
 *      communication error has occurred.
*/
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mlist(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a request for a recursive list from the CACP server, listing the entire
 * structure of the parameter tree below a certain node.  You probably don't
 * want to do this.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.  The list will start at this node and descend
 *      down the tree.
 * @return A CACP transaction representing the "listr" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_listr(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a request for a recursive list from the CACP server, listing the entire
 * structure of the management tree below a certain node.  You probably don't
 * want to do this.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.  The list will start at this node and descend
 *      down the tree.
 * @return A CACP transaction representing the "mlistr" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mlistr(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a "no-operation" request to the server.  This may be used periodically
 * to prevent an idle session from being timed-out.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "noop" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_noop(cacpdclient_connection_t *conn);

/**
 * Sends a request to "verify access" for a node in the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @param [in] access_type A string describing the type of access desired; it
 *      should contain one of "r", "w" or "x" representing read, write and
 *      execute permissions.
 * @return A CACP transaction representing the "access" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_access(cacpdclient_connection_t *conn, const char *address, const char *access_type);

/**
 * Sends a request to "verify access" for a node in the management tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @param [in] access_type A string describing the type of access desired; it
 *      should contain one of "r", "w" or "x" representing read, write and
 *      execute permissions.
 * @return A CACP transaction representing the "maccess" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_maccess(cacpdclient_connection_t *conn, const char *address, const char *access_type);

/**
 * Sends a request to add a new node to the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest (the parent node
 *      under which the new node is to be added).. Components of this
 *      path must be URL-encoded.
 * @param [in] value The name of the node to add.
 * @return A CACP transaction representing the "add" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_add(cacpdclient_connection_t *conn, const char *address, const char *value);

/**
 * Sends a request to remove a node from the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest (the container node
 *      from which a child is to be deleted). Components of this path must be
 *      URL-encoded.
 * @param [in] value The name of the node to remove.
 * @return A CACP transaction representing the "del" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_del(cacpdclient_connection_t *conn, const char *address, const char *value);

/**
 * Sends a request to rename a parameter tree node.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest (the container node
 *      from which a child is to be deleted). Components of this path must be
 *      URL-encoded.
 * @param [in] old_name The name of the existing node.
 * @param [in] new_name The desired new name for the node..
 * @return A CACP transaction representing the "move" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_move(cacpdclient_connection_t *conn, const char *parent_address, const char *old_name, const char *new_name);

/**
 * Sends a request to atomically apply a block of ADD, DEL, SET and MOVE
 * operations on the parameter tree, such that each command is performed only
 * if the entire block succeeds as a whole.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @warning This has not been verified to work.
 * @return A CACP transaction representing the "submit" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_submit(cacpdclient_connection_t *conn, cacpdclient_submitqueue *sq);

/**
 * Sends a request for attributes of a given node in the parameter tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @return A CACP transaction representing the "attr" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_attr(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends a request for attributes of a given node in the management tree.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @param [in] address The address of the node of interest. Components of this
 *      path must be URL-encoded.
 * @return A CACP transaction representing the "mattr" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_mattr(cacpdclient_connection_t *conn, const char *address);

/**
 * Sends the "CURRENT" command to the CACP server, to determine whether this session is
 * "up-to-date" with respect to the latest COMMIT.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "current" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_current(cacpdclient_connection_t *conn);

/**
 * Sends a revert request to the CACP server.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "revert" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_revert(cacpdclient_connection_t *conn);

/**
 * Sends a commit request to the CACP server.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "commit" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_commit(cacpdclient_connection_t *conn);

/**
 * Sends a dirty request to the CACP server.
 *
 * @param [in] conn The CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "dirty" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_dirty(cacpdclient_connection_t *conn);

/**
 * Sends an "EVAL" command to the CACP server.
 *
 * This is only for debugging.  Don't use it.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_eval(cacpdclient_connection_t *conn, const char *expr);

/**
 * Sends a "FUNCLIST" command to the CACP server.
 *
 * This is only for debugging.  Don't use it.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_funclist(cacpdclient_connection_t *conn);

/**
 * Sends an "UPDATE" command to the CACP server, requesting that a non-dirty
 * session be updated to the latest configuration.
 *
 * @param [in] conn the CACP connection on which the command is to be sent.
 * @return A CACP transaction representing the "update" request, or NULL if a
 *      communication error has occurred.
 */
__attribute__((warn_unused_result))
cacpdclient_transaction *cacpdclient_send_update(cacpdclient_connection_t *conn);

/**
 * Checks whether an "UPDATE" command actually updated any data in the CACP
 * server's session.
 *
 * @param [in] trans The "UPDATE" transaction.
 * @return Nonzero if the CACP server modified some data in our session during
 *	the update.
 */
int cacpdclient_update_data_changed(cacpdclient_transaction *trans);

/**
 * Create a new submit queue.  This object is used to accumulate commands that
 * are later sent to a CACP server using \cacpdclient_send_submit().
 *
 * This function does not perform any communication with any server.
 */
struct cacpdclient_submitqueue *create_submitqueue();

/**
 * Clear a submit queue.  This object is used to clear the submit queue
 * are send a return valeu 1 which is fixed.
 *
 * This function does not perform any communication with any server.
 */
int clear_submitqueue(cacpdclient_submitqueue *sq);

/**
 * Appends an ADD command to the given Submit Queue, to add a new node when
 * the queue is passed to cacpdclient_submit.
 *
 * @param [in] container_address The address of the container into which thei
 *      new node is to be added.
 * @param [in] new_name The name of the new node to add.
 * @return An integer identifier for the subcommand.  The response for this
 *      subcommand may be retrieved using this identifier.
 */
int cacpdclient_submit_add(cacpdclient_submitqueue *sq,
    const char *container_address, const char *new_name);

/**
 * Appends a DEL command to the given Submit Queue, to delete an existing node
 * when the queue is passed to cacpdclient_submit.
 *
 * @param [in] container_address The address of the container from which the node is to
 *      be deleted.
 * @param [in] name The name of the existing node to delete.
 * @return An integer identifier for the subcommand.  The response for this
 *      subcommand may be retrieved using this identifier.
 */
int cacpdclient_submit_del(cacpdclient_submitqueue *sq,
    const char *container_address, const char *name);

/**
 * Appends a SET command to the given Submit Queue, to set a new value into a
 * node when the queue is passed to cacpdclient_submit.
 *
 * @param [in] address The address of the node which is to be changed.
 * @param [in] new_value The new value to set into the node.
 * @return An integer identifier for the subcommand.  The response for this
 *      subcommand may be retrieved using this identifier.
 */
int cacpdclient_submit_set(cacpdclient_submitqueue *sq, const char *address,
    const char *new_value);

/**
 * Appends a MOVE command to the given Submit Queue, to rename an existing
 * node when the queue is passed to cacpdclient_submit.
 *
 * @param [in] container_address The address of the container in which the
 *      target node exists.
 * @param [in] old_name The name of the existing node.
 * @param [in] new_name The new name for the node.
 * @return An integer identifier for the subcommand.  The response for this
 *      subcommand may be retrieved using this identifier.
 */
int cacpdclient_submit_move(cacpdclient_submitqueue *sq,
    const char *container_address, const char *old_name, const char *new_name);

/**
 * Obtain the result code for a subcommand in a SUBMIT transaction.
 *
 * @param [in] trans The SUBMIT transaction.
 * @param [in] id The integer identifier from the appropriate
 *      cacpdclient_submit_xxxx call.
 * @return CACP_RESULT_INVALID An invalid response or no response was received.
 *      CACP_RESULT_OK The subcommand was successful.
 *      CACP_RESULT_ERR The subcommand was unsuccessful.
 */
enum cacpdclient_transaction_result_code cacpdclient_submit_result_for_subcommand(
    cacpdclient_transaction *trans, int id);

/**
 * Obtain the error response for a subcommand in a SUBMIT transaction.
 *
 * @param [in] trans The SUBMIT transaction.
 * @param [in] id The integer identifier from the appropriate
 *      cacpdclient_submit_xxxx call.
 * @return The error data for the requested subresponse, or NULL if no verbose
 *      error data is contained within that subresponse.
 */
cacpdclient_verbose_error_data *cacpdclient_submit_error_for_subcommand(
    cacpdclient_transaction *trans, int id);

/**
 * Obtain the error response for a subcommand in a SUBMIT transaction.
 *
 * @param [in] trans The SUBMIT transaction.
 * @param [in] id The integer identifier from the appropriate
 *      cacpdclient_submit_xxxx call.
 * @return The error data for the requested subresponse, or NULL if no verbose
 *      error data is contained within that subresponse.
 *
 * @note This API is ugly and is only here for compatibility.  Please don't
 *      use it in new code.
 */
GSList *cacpdclient_submit_error_for_subcommand_as_list(
    cacpdclient_transaction *trans, int id);

/**
 * From an ATTR or MATTR transaction, obtain the set of operations available
 * on the node on which the ATTR or MATTR was performed.
 *
 * @param [in] trans The ATTR or MATTR transaction.
 * @return A bit-field of the operations available at the node.  Any of the
 *      values from enum cacpdclient_node_operations might be set:
 *      CACPCLIENT_NODE_ADD, CACPCLIENT_NODE_DEL, CACPCLIENT_NODE_GET,
 *      CACPCLIENT_NODE_SET, CACPCLIENT_NODE_LIST, CACPCLIENT_NODE_MOVE,
 *      CACPCLIENT_NODE_ADMIN, CACPCLIENT_NODE_INFO, CACPCLIENT_NODE_HELP.
 */
int cacpdclient_attr_operations(cacpdclient_transaction *trans);

/**
 * Create a submit queue for handling tsapi submit calls.
 *
 * @param [in] void
 * @return 1 on success 0 on failure
 * 
 */
int create_tsapi_subque(void);

/**
 * Kill a submit queue handling tsapi submit calls.
 *
 * @param [in] void
 * @return void
 * 
 */
void cacpdclient_clear_tsapi_submitqueue(void);

/**
 * Submit the tsapi submit queue for handling tsapi submit calls.
 *
 * @param [in] conn the CACP connection on which the command is to be sent.
 * @return NULL on success or an Error string with subid on which failed.
 * 
 */
char *cacpdclient_tsapi_submit(cacpdclient_connection_t *conn);

#endif /* CACP_COMMANDS_H_INCLUDED */
