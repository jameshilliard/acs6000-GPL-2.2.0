/**
 * @file cacpdparser.h Header for cacpdparser.c
 *
 * @Authors Oliver Kurth <oliver.kurth@avocent.com>
 * 			Hugh Nelson  <hugh.nelson@avocent.com>
 */

/* -*- linux-c -*- */
/*
    Copyright (C) 2007 Avocent Corp.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CACPDPARSER_H_INCLUDED
#define CACPDPARSER_H_INCLUDED

#include <glib/gslist.h>

#include <libcacpdparser/error.h>
#include <libcacpdparser/transaction.h>
#include <libcacpdparser/connection.h>
#include <libcacpdparser/commands.h>

/*
 * Usage:
 * Generally, call one of the cacpdclient_send_xxx commands, keep a copy of
 * the cacpdclient_transaction object around until you need the response,
 * then call one of the accessors (the functions starting with
 * cacpdclient_transaction_get_xxx, listed below with an annotation that
 * they will block until the response is received), and you can obtain the
 * result (OK/ERR/BAD/NO/INUSE) and query the response data from the CACP
 * server.  When you have what you need, destroy the transaction.  You can
 * have as many transactions in-flight as you want... it doesn't need to be
 * request->response->request->response...
 *
 * A very rough example:
 *
 *      trans1 = cacpdclient_send_xxx(conn, ...)
 *      trans2 = cacpdclient_send_yyy(conn, ...)
 *      ...
 *      if (CACP_RESULT_OK == cacpdclient_transaction_get_result_code(trans1)){
 *          // trans1 was successful
 *          char *foo = cacpdclient_transaction_get_value(trans1);
 *      }
 *      cacpdclient_destroy_transaction(trans1);
 *      if (CACP_RESULT_OK == cacpdclient_transaction_get_result_code(trans2)){
 *          // trans2 was successful
 *          // do something with it (if you want to)
 *      }
 *      cacpdclient_destroy_transaction(trans2);
 *      ...
 *
 * CAVEATS (FIXME FIXME FIXME)
 *
 * You *MUST* collect the responses to the commands in the same order that
 * they were sent.  This is an artefact of the refactoring that's been
 * happening lately... currently all clients request the responses immediately,
 * so no-one is breaking this rule.  This is a big FIXME!  Until it's fixed,
 * just keep requesting results in the same order the transactions were sent,
 * otherwise they won't match up properly.
 *
 * Currently there is no logic to ensure that you are using the appropriate
 * cacpdclient_transaction_get_xxx function for the type of transaction which
 * was sent.  This may be added in future.  Currently there is also no
 * DOCUMENTATION of which function to use ;-(
 */

/*
 * Refactoring TODO list:
 *
 * More command-specific cacpdclient_transaction_get_xxx functions, particularly
 * for the SUBMIT command.
 *
 * Hide the "subid" concept from the clients... they should just ask for what
 * they need, not needing to know which subidentifier to request.
 */


/**
 * Create a connection to a CACP server.  This is currently hard-coded to
 * connect to a Unix socket at "/dev/cacpd"; this may be overridden at
 * build time, but not at run-time.
 *
 * @return An object representing the connection to the CACP server, or
 *      NULL if a connection could not be established.
 */
__attribute__((warn_unused_result))
cacpdclient_connection_t* cacpdclient_connect(void);

/**
 * Disconnect a session from a CACP server, freeing the resources used for the
 * local side of the connection.
 *
 * @param [inout] conn The CACP client connection object to disconnect.  After
 *      this function is called, any access to this parameter will be invalid.
 */
void cacpdclient_disconnect(cacpdclient_connection_t *conn);

/**
 * Makes a copy of all the response text in the given list.
 * If you (humble caller) want a copy of all of the list contents, this
 * function provides an easy way to obtain it.  Use this if you still want to
 * use one of these lists after the transaction has been destroyed, since
 * calling \cacpdclient_destroy_transaction will invalidate all the memory
 * that stores the CACP response.
 *
 * @param [in] orig A list of strings, as returned by one of the
 *      \cacpdclient_transaction_get_list_for_id or \cacpd_transaction_get_choices
 *      family of functions.
 * @return The same GSList as was passed in, but upon return, the caller will
 *      own the list contents as well.
 */
GSList *cacpdclient_copy_response_list(GSList *orig);

#endif /* CACPDPARSER_H_INCLUDED */
