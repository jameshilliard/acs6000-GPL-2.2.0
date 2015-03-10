#include "adsap2_fn.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

void readpem( adsap2_pem* pem, char* filename )
{
	pem->pem_length = -1;
	int fd = open( filename, O_RDONLY );
	if ( fd >= 0 )
	{
		int sz = 0;
		int buffsize = 1024;
		void* data = malloc( buffsize );
		int r = read( fd, data, 1024 );
		while ( r > 0 )
		{
			sz += r;
			if ( buffsize-sz == 0 )
			{
				buffsize += 1024;
				data = realloc( data, buffsize );
			}
			r = read( fd, data + sz, buffsize-sz );
		}
		pem->pem_length = sz;
		pem->pem = realloc( data, sz );
		close( fd );
	}
}

int main( int argc, char* argv[] )
{
	int success = 1;
	adsap2_result* result = air_alloc( NULL, sizeof( adsap2_result ) );
	if ( argc == 5 )
	{
		adsap2_pem pem;
		readpem( &pem, argv[2] );
		if ( pem.pem_length == -1 )
		{
			printf( "Attempting to Authenticate and Authorize access to %s using mode %s with username %s and password %s...\n", argv[4], argv[3], argv[1], argv[2] );
			success = adsap2_authenticate_password( "adsap2", argv[1], argv[2], argv[3], argv[4], result );
		}
		else
		{
			printf( "Attempting to Authenticate and Authorize access to %s using mode %s with username %s and pem file %s...\n", argv[4], argv[3], argv[1], argv[2] );
			success = adsap2_authenticate_pem( "adsap2", (strlen(argv[1])>0)?argv[1]:NULL, pem, (strlen(argv[3])>0)?argv[3]:NULL, (strlen(argv[4])>0)?argv[4]:"", result );
		}
	}
	else if ( argc == 4 )
	{
		printf( "Attempting to Authorize access to %s using mode %s with handle %s...\n", argv[3], argv[2], argv[1] );
		success = adsap2_authorize( "adsap2", atoi( argv[1] ), argv[2], argv[3], result );
	}
	else
	{
		printf( "Password: %s [username] [password] [mode] [connection]\n", argv[0] );
		printf( "PEM: %s [username] [pem file] [mode] [connection]\n", argv[0] );
		printf( "Authorize: %s [handle] [mode] [connection]\n", argv[0] );
	}
	if ( success == 0 )
	{
		printf( "ADSAP2 Result: %d\n", result->status );
		if ( result->status == 0 )
		{
			printf( "Auth Handle: %d\n", result->auth_handle );
			printf( "Username: %s\n", result->user );
			printf( "Connection Id: %s\n", result->connectionid );
			printf( "Preemption Level: %d\n", result->preemption_level );
			printf( "Session Mode: %s\n", result->session_mode );
			printf( "Session Type: %s\n", result->session_type );
			printf( "Appliance Rights: %s\n", result->appliance_rights );
			printf( "Target Rights: %s\n", result->target_rights );
		}    
	}
	else
	{
		printf( "Call Failed: %d\n", success );
	}
	air_free( result );
	
	
}
