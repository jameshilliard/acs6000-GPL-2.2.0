#include <stdlib.h>
#include <string.h>
#include <glib/gslist.h>

#include <dlog.h>

#include "libcacpdparser/cacpdparser.h"

#define DLOG_CONTEXT "cacpclient"

struct cacpdclient_verbose_error_data {
    char *i18n_key;
    GSList *args;
};

cacpdclient_verbose_error_data *cacpdclient_create_verbose_error_data(
    const char const *i18n_key, GSList *args)
{
    cacpdclient_verbose_error_data *retval = (cacpdclient_verbose_error_data *)
        malloc(sizeof(cacpdclient_verbose_error_data));

    if (!retval)
        return retval;

    retval->i18n_key = strdup(i18n_key);
    retval->args = cacpdclient_copy_response_list(g_slist_copy(args));
    return retval;
}

char *cacpdclient_verbose_error_key(cacpdclient_verbose_error_data *err)
{
    return err->i18n_key;
}

/*
 * Perform substitution of arguments into the internationalized error message.
 */
char *cacpdclient_verbose_error_format(cacpdclient_verbose_error_data *err,
    const char const *fmt)
{
    const char *src = fmt;
    int required_len = 0;

    while (*src) {
        if ('{' == *src) {
            char *end;
            int index = strtol(src + 1, &end, 10);

            if (('}' == *end) && (index > 0)) {
                /* Found {n} */
                char *insert = g_slist_nth_data(err->args, index - 1);
                if (insert) {
                    /* We got a string to insert here. */
                    required_len += strlen(insert);
                    src = end + 1;
                    continue;
                }
            }
        }

        src++;
        required_len++;
    }

    char *result = (char *)malloc(required_len + 1);
    char *dest = result;
    src = fmt;

    while (*src) {
        if ('{' == *src) {
            char *end;
            int index = strtol(src + 1, &end, 10);

            if (('}' == *end) && (index > 0)) {
                /* Found {n} */
                const char *insert = g_slist_nth_data(err->args, index - 1);
                if (insert) {
                    /* We got a string to insert here. */
                    int insert_len = strlen(insert);
                    strncpy(dest, insert, insert_len);
                    dest += insert_len;
                    src = end + 1;
                    continue;
                }
            }
        }

        *(dest++) = *(src++);
    }
    *dest = '\0';

    if (dest != result + required_len)
        dlog_printf(DLOG_ERROR, DLOG_CONTEXT, "I18N: Internal error.  Buffer size mismatch.");

    return result;
}

char *cacpdclient_verbose_error_description(cacpdclient_verbose_error_data *err)
{
    if (!err)
        return NULL;

    int required_len = strlen(err->i18n_key) + strlen("key=, data={}");

    GSList *l;
    for (l = err->args; l; l = l->next)
        required_len += strlen((char *)l->data) + 1;

    char *result = malloc(required_len);
    strncpy(result, "key=", 4);
    char *p = result + 4;

    int len = strlen(err->i18n_key);
    strncpy(p, err->i18n_key, len);
    p += len;

    strncpy(p, ", data={", 8);
    p += 8;

    for (l = err->args; l; l = l->next) {
        if (l != err->args)
            *(p++) = ' ';

        len = strlen(l->data);
        strncpy(p, (char *)l->data, len);
        p += len;
    }
    *(p++) = '}';
    *p = '\0';

    return result;
}

void cacpdclient_destroy_verbose_error_data(cacpdclient_verbose_error_data *err)
{
    GSList *p;
    for (p = err->args; p; p = p->next)
        free(p->data);

    g_slist_free(err->args);
    free(err->i18n_key);
    free(err);
}

