/**
 * @file response_ut.c
 *
 * Basic tests for response.c.  
 *
 * @todo Expanded into a formal unit test harnass.
 *
 * @author Hugh Nelson
 *
 */

#include "response.h"

/**
 * Basic tests for response.c
 */
int main( void )
{
	GString* one   = newString( "One"   );
	GString* two   = newString( "Two"   );
	GString* three = newString( "Three" );

	GList*   myList    = NULL;
	GList*   myDoc     = NULL;
	element* myElement = NULL;

	myList = prependStringToList( myList, one   );
	myList = prependStringToList( myList, two   );
	myList = prependStringToList( myList, three );

	myElement = newMultiValueElement( myList, "Numbers!!!" );
	myDoc     = appendElementToDocument( myDoc, myElement );

	myElement = newSingleValueElement( "Single value.", "Four" );
	myDoc     = appendElementToDocument( myDoc, myElement );

	displayDocument( myDoc );

	destroyDocument( myDoc );

	return 0;
}

