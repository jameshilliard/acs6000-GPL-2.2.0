/**
 * @file urlcoding.c Applies and removes url-encoding from strings.
 * @ingroup libcacpdparser
 */

#include <ctype.h>
#include <string.h>
#include <glib.h>

#include "libcacpdparser/urlcoding.h"

/**
 * Converts a hex charecter string into an integer value.
 *
 * @param s The hex charecter string to be converted.
 *
 * @return The integer value.
 */
static gint htoi( const gchar* s )
{
	gint c = ((guchar*) s )[ 0 ];

	if( isupper( c ) )
		c = tolower( c );

	gint value = ( c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = ( (guchar*) s )[ 1 ];

	if( isupper( c ) )
		c = tolower( c );

	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

	return ( value );
}

/// An unsigned charecter array holding all valid hex digits.
static guchar hexchars[] = "0123456789ABCDEF";

/**
 * Applies url-encoding to a string.
 *
 * @param s A string to encode.
 *
 * @return A url-encoded string, this string must be freed.
 */
char* url_encode( const char* s )
{
	register guchar c     = 0;
	GString*        to    = g_string_sized_new( strlen( s ) );
	guchar*         from  = (guchar*) s;

	while( *from ) 
	{
		c = *from++;

		if ( c == ' ' ) 
			g_string_append_unichar( to, '+' );
		else if ( ( c < '0' && c != '-' ) || 
				    ( c < 'A' && c > '9' ) ||
		   		 ( c > 'Z' && c < 'a' && c != '_' ) || 
					 ( c > 'z' ) ) 
		{
			g_string_append_unichar( to, '%' );
			g_string_append_unichar( to, hexchars[ c >> 4 ] );
			g_string_append_unichar( to, hexchars[ c & 15 ] );
		} 
		else 
			g_string_append_unichar( to, c );
	}

	return g_string_free( to, FALSE );
}

/**
 * Removes url-encoding from a string.
 *
 * @param str A string to decode.
 *
 * @return A pointer to the decoded string,
 *         this pointer must be freed.
 */
char* url_decode( const char* str )
{
	const char*    c  = str;
	GString* to = g_string_new( NULL );

	while( *c ) 
	{
		if ( *c == '+' ) 
			g_string_append_unichar( to, ' ' );
		else if( *c == '%' && 
				   *( c + 1 ) && 
				   *( c + 2 ) && 
					isxdigit( (int) *( c + 1 ) ) && 
					isxdigit( (int) *( c + 2 ) ) ) 
		{
			g_string_append_c( to, htoi( c + 1 ) );
			c += 2;
		} 
		else
			g_string_append_c( to, *c );

		++c;
	}

	return g_string_free( to, FALSE );
}

