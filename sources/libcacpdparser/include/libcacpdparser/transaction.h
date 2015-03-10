#ifndef CACPDCLIENT_TRANSACTION_H_INCLUDED
#define CACPDCLIENT_TRANSACTION_H_INCLUDED

#include <glib/gslist.h>

#include <libcacpdparser/error.h>

/**
 * Opaque handle representing a request and its corresponding reply and status.
 */
typedef struct cacpdclient_transaction cacpdclient_transaction;

/**
 * Enumerates the possible result codes returned for each CACP request.
 */
enum cacpdclient_transaction_result_code {
    CACP_RESULT_OK,  /**< The request completed successfully. */
    CACP_RESULT_ERR, /**< The request encountered an error while in progress (i.e. validation). */

    CACP_RESULT_MALFORMED_REQUEST, /**< The request was malformed. */
    CACP_RESULT_MALFORMED_RESPONSE, /**< An invalid response was received. */
    CACP_RESULT_CONNECTION_FAILED, /**< The connection was lost or closed. */
    CACP_RESULT_SESSION_TERMINATED, /**< The session has been terminated by the server. */
    CACP_RESULT_NODE_EXISTS, /**< An "ADD" failed because the node already exists. */
    CACP_RESULT_NODE_NOT_FOUND, /**< The node address was not found. */
    CACP_RESULT_SANITY_CHECK_FAILED, /**< A generic sanity-check failed. */
    CACP_RESULT_PERMISSION_DENIED, /**< Permission denied. */
    CACP_RESULT_AUTHENTICATION_FAILED, /**< Authentication failed. */
    CACP_RESULT_COMMIT_CONFLICT, /**< Conflict between concurrent writers. */

    CACP_RESULT_NONE /**< No response has been received yet. */
};


/**
 * Gets the value of a "simple" response to a transaction.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the value.
 * @return A string containing the value within the response, or NULL if no
 *      suitable string was found in the response.  The caller does not own
 *      the data that the pointer references.  The data becomes invalid when
 *      the transaction is destroyed.
 */
char *cacpdclient_transaction_get_value(cacpdclient_transaction *trans);


/**
 * Gets the list of all response lines resulting from a transaction.
 *
 * This function simply concatenates all of the lines of response content
 * (IDs are not included).  You should probably not use this function,
 * as its behavior will be unpredictable as the protocol is changed in
 * the future.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the list.
 * @return A GSList of (const char *) elements, consisting of the ordered lines
 *      of content received from the CACP server.  The caller owns the pointer
 *      list, but DOES NOT own the pointed-to text.  Caller must free the
 *      returned list using \g_slist_free(), or alternatively may call
 *      \cacpdclient_copy_response_list to "own" a copy of the strings that
 *      will live beyond the destruction of the transaction.
 */
GSList *cacpdclient_transaction_get_list(cacpdclient_transaction *trans);

/**
 * Gets a list of response lines that match exactly a given subresponse ID.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the list.
 * @param [in] id The identifier for the subresponse to list, or NULL to
 *      simply get all subresponses in one list.
 * @return The list of response lines (const char *) with the given id.  The caller
 *      owns the pointer list, but DOES NOT own the pointed-to text.  Caller
 *      must free the returned list using \g_slist_free(), or alternatively may
 *      call \cacpdclient_copy_response_list to "own" a copy of the strings
 *      that will live beyond the destruction of the transaction.
 */
GSList *cacpdclient_transaction_get_list_for_id(cacpdclient_transaction *trans, const char const *id);

/**
 * Gets a single string containing the response lines matching a given
 * subresponse ID.  The lines will be joined by newlines (\n) into a single
 * string.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the list.
 * @param [in] id The identifier for the subresponse to list, or NULL to
 *      simply get all subresponses in one list.
 * @return A malloc()ed string containing the value at the given id.
 */
char *cacpdclient_transaction_get_value_for_id(cacpdclient_transaction *trans, const char const *id);

/**
 * Gets a list of response lines that contain a subresponse ID that starts with
 * the given substring.  The response subidentifiers are NOT included.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the list.
 * @param [in] id The identifier for the start of the subresponse ID to match.
 * @return The list of response lines (const char *) with the given id.  The caller
 *      owns the pointer list, but DOES NOT own the pointed-to text.  Use
 *      g_slist_free to free the list, or cacpdclient_copy_response_list to
 *      own the list AND its contents.
 */
GSList *cacpdclient_transaction_get_list_for_id_starting_with(cacpdclient_transaction *trans, const char const *id);

/**
 * Gets the list of choices from the response document.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction from which to obtain the list.
 * @param [in] id The identifier for the start of the subresponse ID to match.
 * @return The list of strings (const char *) containing the choices available
 *      at the node named in the ATTR command.  The caller
 *      owns the pointer list, but DOES NOT own the pointed-to text.  Use
 *      g_slist_free to free the list, or cacpdclient_copy_response_list to
 *      own the list AND its contents.
 */
GSList *cacpdclient_transaction_get_choices(cacpdclient_transaction *trans);

/**
 * Obtain the result code from the given transaction.
 *
 * This function will block if necessary to obtain a response to the
 * request.
 *
 * @param [in] trans The transaction for which the result code is requested.
 * @return If the transaction has completed normally, one of CACP_RESULT_OK,
 *      CACP_RESULT_ERR, CACP_RESULT_BAD, CACP_RESULT_NO, CACP_RESULT_INUSE;
 *      If no response has been received yet (including a communication
 *      failure), CACP_RESULT_NONE; If a malformed response is received,
 *      this function *may* return CACP_RESULT_INVALID.
 */
enum cacpdclient_transaction_result_code cacpdclient_transaction_get_result_code(cacpdclient_transaction *trans);

/**
 * Obtains the text corresponding to the given result code, suitable for logging.
 * @param [in] result_code The result code for which the text description is
 *      required.
 * @return A pointer to a static string containing the text of the CACP result.
 */
const char const *cacpdclient_result_code_description(enum cacpdclient_transaction_result_code result_code);

/**
 * Obtain a textual description of the command result, suitable for logging.
 *
 * @param [in] trans The transaction for which the description is requested.
 * @return A malloc()ed string describing the transaction's result.  The caller
 *	must free() this when it is no longer needed.
 */
char *cacpdclient_transaction_get_result_description(cacpdclient_transaction *trans);

/**
 * Obtains the verbose error response from the given transaction.
 *
 * @param [in] trans The transaction for which the error response is desired.
 * @return The verbose error structure, or NULL if no verbose error information
 *      exists for the given transaction.
 */
cacpdclient_verbose_error_data *cacpdclient_transaction_error(
    cacpdclient_transaction *trans);

/**
 * Destroys the entire transaction record, including the request and response
 * data.
 *
 * After calling this function, any lists obtained through any of the following
 * functions will no longer be valid:
 *      * \cacpdclient_transaction_get_list
 *      * \cacpdclient_transaction_get_list_for_id
 *      * \cacpdclient_transaction_get_list_for_id_starting_with
 *      * \cacpdclient_transaction_get_choices
 * @param [inout] trans The transaction to be destroyed.
 */
void cacpdclient_destroy_transaction(cacpdclient_transaction *trans);

#endif /* CACPDCLIENT_TRANSACTION_H_INCLUDED */
