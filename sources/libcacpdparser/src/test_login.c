/**
 * @file test_login.c Test harnass for flex_client.c that was used
 * during inital development, also used as an example of how to
 * use the library.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libcacpdparser/cacpdparser.h"

/**
 * CACPD parser test harnass.
 */
int main( void )
{
	cacpdclient_transaction *trans, *trans2, *trans3;
        GSList* lineList;
        gchar *value;
	cacpdclient_connection_t *conn;
	char *session_id;

	conn = cacpdclient_connect();
	
	trans = cacpdclient_login( conn, "root", "root" );
        
//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);

	session_id = cacpdclient_connection_get_sessionid(conn);

	printf( "Login done\n" );
	cacpdclient_disconnect(conn);


	conn = cacpdclient_connect();

	trans = cacpdclient_resume(conn, session_id);
//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);
	printf( "Resume done\n" );

	trans = cacpdclient_send_get(conn, "network.hostname" );
    value = cacpdclient_transaction_get_value(trans);
    printf("cacpdclient_transaction_get__value returned value=%s\n",value);
//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);

	printf( "PIPE *******************\n" );
	trans = cacpdclient_send_set( conn, "network.hostname", "blammo" );
	trans2 = cacpdclient_send_get( conn, "network.hostname" );
	trans3 = cacpdclient_send_list( conn, "network" );

//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);

//	displayResponse(cacpdclient_get_response(trans2));
	cacpdclient_destroy_transaction(trans2);

//	displayResponse(cacpdclient_get_response(trans3));
	cacpdclient_destroy_transaction(trans3);

	printf( "PIPE *******************\n" );

	trans = cacpdclient_send_attr( conn, "network.interface.eth0.active" );
    lineList = cacpdclient_transaction_get_choices(trans);
    printf("cacpdclient_transaction_get_choices returned value=%s\n",(char *)lineList->data);
//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);

	trans = cacpdclient_logout(conn);
//	displayResponse(cacpdclient_get_response(trans));
	cacpdclient_destroy_transaction(trans);
	printf( "logout done\n" );

	cacpdclient_disconnect(conn);
	free(session_id);

	return 0;
}
