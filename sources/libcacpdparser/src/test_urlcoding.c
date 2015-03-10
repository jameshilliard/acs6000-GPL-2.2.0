/**
 * @file test_urlcoding.c Test harnass for urlcoding.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "urlcoding.h"

int main( void )
{
	char* start = "This is a string.  Hopefully, this string will change--as expected!";

	printf( "%s\n", start );

	char* second = url_encode( start );
	printf( "%s\n", second );
	
	char* stop = url_decode( second );
	printf( "%s\n", stop );

	free( second );
	free( stop );

	return 0;
}
