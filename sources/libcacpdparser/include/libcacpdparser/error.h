#ifndef CACPDPARSER_ERROR_H_INCLUDED
#define CACPDPARSER_ERROR_H_INCLUDED

typedef struct cacpdclient_verbose_error_data cacpdclient_verbose_error_data;

/**
 * Creates a "verbose error response" structure.  Use this if you already have
 * a copy of the raw (text) data for the verbose error response, and you want
 * to construct a \cacpdclient_verbose_error_data structure from it.
 *
 * @param [in] i18n_key A string containing the internationalization key for
 *	the error (i.e. "error.network.ip").
 * @param [in] args A GSList of strings which contain the arguments to the
 *	verbose error response.  The first element of the list will correspond
 *	to "{1}" when the error message is formatted, and so on.
 * @return The error data structure.
 */
cacpdclient_verbose_error_data *cacpdclient_create_verbose_error_data(
    const char const *i18n_key, GSList *args);

/**
 * Obtains the internationalization key for the given error.  For example, the
 * result might be a string such as "error.network.ip".
 *
 * @param [in] err The error data from which the internationalization key is
 *      to be extracted.
 * @return A pointer to the internationalization key string.  The caller does
 *      not own the pointer.  The pointer is valid until the error data
 *      structure is destroyed.
 */
char *cacpdclient_verbose_error_key(cacpdclient_verbose_error_data *err);

/**
 * Given the internationalized template text, produces the formatted and
 * internationalized error string.
 *
 * @param [in] err The error data from which the message is to be constructed.
 * @param [in] fmt The format string, in which the replaceable parameters are
 *      indicated by numbers in brackets: "{1}", "{2}", etc.
 * @return A malloc()ed string with the appropriate substitutions performed.
 */
char *cacpdclient_verbose_error_format(cacpdclient_verbose_error_data *err,
    const char const *fmt);

/**
 * For debug/logging, obtain a simple copy of the contents of this error
 * response.  The formatting of this string is unspecified, but should be
 * generally suitable for logging.
 *
 * @param [in] err The error data to query.
 * @return A malloc()ed string describing the error.
 */
char *cacpdclient_verbose_error_description(cacpdclient_verbose_error_data *err);

/**
 * Disposes of the resources used for storing a verbose error response.
 *
 * @param [in] err The error data to destroy.
 */
void cacpdclient_destroy_verbose_error_data(cacpdclient_verbose_error_data *err);

#endif /* CACPDPARSER_ERROR_H_INCLUDED */
