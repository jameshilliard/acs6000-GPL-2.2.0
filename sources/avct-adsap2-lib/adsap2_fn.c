#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "adsap2_fn.h"

static int adsap2_has_right( char* rights, char* right )
{
  if ( ( rights == NULL ) || ( right == NULL ) )
  {
    return ( 0 );
  }
  
  int nRightLen = strlen( right );
  
  char ch = *rights;
  while ( ch != 0 )
  {
    if ( ( ch != ',' ) && ( ch != ' ' ) )
    {
      // Found Start of Right
      char* end = rights;
      int len = 0;
      char ch2 = *end;
      while ( ( ch2 != ',' ) && ( ch2 != 0 ) && ( ch2 != ' ' ) )
      {
        end++;
        len++;
        ch2 = *end;
      }
      if ( ( len == nRightLen ) && ( strncmp( right, rights, len ) == 0 ) )
      {
        return ( 1 );
      }
      rights = end;
    }
    else
    {
      // Skip space or Comma
      rights++;
    }
    ch = *rights;
  }
  
  return ( 0 );
}

int adsap2_has_target_right( adsap2_result* result, char* right )
{
  return ( adsap2_has_right( result->target_rights, right ) );
}

int adsap2_has_appliance_right( adsap2_result* result, char* right )
{
  return ( adsap2_has_right( result->appliance_rights, right ) );
}

