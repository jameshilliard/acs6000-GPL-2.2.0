/**
 * @file flex_scanner_ut.c Flex scanner unit tests.
 *
 * @author Hugh Nelson
 *
 * @todo Fully implement the unit tests.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <CUnit/Basic.h>

#include "flex_client.h"
#include "response.h"

/// The CACPD pid.
pid_t cacpd_pid;

/// The session id.
gchar* sid;

//  CACPD Utils  -----------------------------------

/**
 * Starts up CACPD.
 *
 * @return An error code.
 */
int cacpd_start( void )
{
	cacpd_pid = fork();

	///@todo For now, we are assuming that cacpd will always
	/// start correctly, this will need to be fixed.

	if( cacpd_pid )
	{
		sleep( 1 );
		init_parser();
		return CUE_SUCCESS;
	}
	else
	{
		chdir( "/home/hugh/dev/cacpd/cyconfd" );

		system( "./src/cacpd " 
				  "-c cacpd-test.conf "
				  "-p param.conf "
				  "-m modules.conf "
				  "-C /home/hugh/dev/cacpd/cyconfd/examples/ "
				  "-n > /dev/null 2>&1" );

		exit( 0 );
	}
}

/**
 * Stops CACPD.
 *
 * @return An error code.
 */
int cacpd_stop( void )
{
	destroy_parser();
	system( "/home/hugh/dev/cacpd/cyconfd/src/cacpd -c"
			  "/home/hugh/dev/cacpd/cyconfd/cacpd-test.conf -k" );
	return CUE_SUCCESS;
}

//
//  Login/Logout init  -----------------------------

/**
 * Initalizes the login suite.
 *
 * @return An error code.
 */
int init_suite_login( void )
{
	sid = NULL;
	return cacpd_connect();
}

/**
 * Cleans the login suite.
 *
 * @return An error code.
 */
int clean_suite_login( void )
{
	return cacpd_disconnect();
}

// 
//  Login/Logout tests  -----------------------------

/// Tests login/logout conditions.
void test_Login( void )
{
	GList*   doc          = login( "root", "root" );
	GString* reply        = NULL;
	int      session_good = FALSE;

//	displayDocument( doc );

	reply  = getSingleValue( doc, "respcode" );
	CU_ASSERT_STRING_EQUAL( reply->str, "OK" ); 
	g_string_free( reply, TRUE );

	reply  = getSingleValue( doc, "perms" );
	CU_ASSERT_STRING_EQUAL( reply->str, "7" ); 
	g_string_free( reply, TRUE );

	reply  = getSingleValue( doc, "session" );

	regex_t regex;
	regcomp( &regex, "[0-9a-f]{16}", REG_EXTENDED | REG_NOSUB );

	session_good = regexec( &regex, reply->str, 0, NULL, 0 );
	regfree( &regex );

	if( session_good )
		sid = g_strdup( reply->str );
	else
		CU_ASSERT_FALSE( regexec( &regex, reply->str, 0, NULL, 0 ) );

	g_string_free( reply, TRUE );
}

/// Tests the inuse response.
void test_LoginInuse( void )
{
	/*
	char* reply = login( "root", "root" );

	CU_ASSERT_STRING_EQUAL( reply, 
			"<?xml version=\"1.0\"?>\n<CACP_REPLY><login_inuse><username>root</username></login_inuse></CACP_REPLY>" );

	free( reply );
	*/

	CU_FAIL( "not implemented" );
}

/// Tests LOGINFORCE
void test_LoginForce( void )
{
	CU_FAIL( "not implemented" );
}

/// Tests logging in twice.
void test_DoubleLogin( void )
{
	GList*   doc   = login( "root", "root" );
	GString* reply = getSingleValue( doc, "respcode" );

	CU_ASSERT_STRING_EQUAL( reply->str, "ERR" ); 
	g_string_free( reply, TRUE );

	reply = getSingleValue( doc, "message" );

	CU_ASSERT_STRING_EQUAL( reply->str, "already logged in" ); 
	g_string_free( reply, TRUE );
}

/// Tests logout.
void test_Logout( void )
{
	GList* doc = logout();
	GString* reply = getSingleValue( doc, "respcode" );
	CU_ASSERT_STRING_EQUAL( reply->str, "OK" ); 
	g_string_free( reply, TRUE );
}

/// Tests resume.
void test_Resume( void )
{

}

// 
// Get/Set init  -----------------------------------

/**
 * Initalizes the get/set suite.
 *
 * @return A status code.
 */
int init_suite_get_set( void )
{
	cacpd_start();
	return CUE_SUCCESS;
}

/**
 * Cleans the get/set suite.
 *
 * @return a status code.
 */
int clean_suite_get_set( void )
{
	return CUE_SUCCESS;
}

//
// Get/Set tests  ----------------------------------
	//printf( "***%s***", reply->str );

/// Tests a simple get command.
void test_GetSimple( void )
{
	get( "network.hostname" );
	GList* doc = get_doc();
	//displayDocument( doc );
	GString* reply = getSingleValue( doc, "values" );
	CU_ASSERT_STRING_EQUAL( reply->str, "foobar" ); 
	g_string_free( reply, TRUE );
}
/*
void test_List( void )
{
	list( "network" );
	GList* doc = get_doc();
	xmlChar*  reply = NULL;
	int       size  = 0;

	xmlDocDumpMemory( doc, &reply, &size );

	printf( "***%s***", reply );
	CU_ASSERT_STRING_EQUAL( reply, 
			"<?xml version=\"1.0\"?>\n<CACP_REPLY><values><value>foobar</value></values></CACP_REPLY>\n" );

	xmlFree( reply );
	xmlFreeDoc( doc );
}

void test_SetSimple( void )
{
	set( "network.hostname", "alpo" );
	GList* doc = get_doc();
	xmlChar*  reply = NULL;
	int       size  = 0;

	xmlDocDumpMemory( doc, &reply, &size );

	CU_ASSERT_STRING_EQUAL( reply, 
			"<?xml version=\"1.0\"?>\n<CACP_REPLY><OK/></CACP_REPLY>\n" );

	xmlFree( reply );
	xmlFreeDoc( doc );

	get( "network.hostname" );
	doc = get_doc();
	reply = NULL;
	size  = 0;

	xmlDocDumpMemory( doc, &reply, &size );

	CU_ASSERT_STRING_EQUAL( reply, 
			"<?xml version=\"1.0\"?>\n<CACP_REPLY><values><value>alpo</value></values></CACP_REPLY>\n" );

	xmlFree( reply );
	xmlFreeDoc( doc );

	set( "network.hostname", "foobar" );
	doc = get_doc();
	xmlFreeDoc( doc );
}
*/
//
// Main  -------------------------------------------

/// Entry point for the flex scanner unit tests.
/// @return A status code.
int main()
{
   CU_initialize_registry();

	// Login / Logout

	CU_pSuite S = CU_add_suite( "login/logout", 
									init_suite_login, 
									clean_suite_login );

	CU_add_test( S, "login",         test_Login       );
	CU_add_test( S, "double login",  test_DoubleLogin );

	// Get / Set

//	S = CU_add_suite( "get/set", 
//									init_suite_get_set, 
//									clean_suite_get_set );

	CU_add_test( S, "get ( simple )", test_GetSimple  );
//	CU_add_test( S, "set ( simple )", test_SetSimple  );
//	CU_add_test( S, "list",           test_List       );
	CU_add_test( S, "logout",         test_Logout     );

	// Run tests

	CU_basic_set_mode( CU_BRM_VERBOSE );
	CU_basic_run_tests();

	return CU_get_error();
}
//
