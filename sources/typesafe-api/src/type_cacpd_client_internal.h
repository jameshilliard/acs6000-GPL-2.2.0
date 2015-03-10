/**
   @file type_cacpd_client.h
   connection to cacpd for the type_safe_api
*/

#ifndef TYPE_CACPD_CLIENT_INTERNAL_H
#define TYPE_CACPD_CLIENT_INTERNAL_H

/**
 * Reconnect to CACPD.
 * @return TS_OK on success, or an error code (CONNECT_FAIL) on failure.
 */
int tsapi_make_session_ready(gsp_tsapi_session_t *sess);

/*
 * Go idle.  If this means that we should disconnect from CACPD,
 * do so.
 * @return TS_OK on success, or a TSAPI error code (i.e. CONNECT_FAIL) on
 *      failure.
 */
int tsapi_make_session_idle(gsp_tsapi_session_t *sess);

#endif
