/*---------------------------------------------------------------------
 * Copyright 2004, Avocent Corporation
 *---------------------------------------------------------------------
 * FILE         : kernelhdr.c
 *
 * DESCRIPTION  : Utility program to prepend a kernel image with a Avocent
 *                header.
 *
 *---------------------------------------------------------------------
 * AUTHOR       : David Stafford
 * DATE         : Feb 26-2004
 *
 *---------------------------------------------------------------------
 * HISTORY
 *
 * DATE         PROGRAMMER        MODIFICATIONS
 * Feb 26-2004  David Stafford    Initial version
 *
 *---------------------------------------------------------------------
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>

#include <types.h>
#include <flashhdr.h>

#define VERSION   "1.0"

static FILE *outFile;       /* flash output file */
static char *outName;

static FILE *kernelFile;    /* KERNEL input file */
static char *kernelName;


/*---------------------------------------------------------------------
 * FUNCTION         : DisplayHelp( void )
 *
 * DESCRIPTION      : This function prints instructions for program
 *                    usage.
 *
 * PARAMETERS       : none
 *
 * RETURNS          : none
 *
 * NOTES            : This function does not return.
 *
 *---------------------------------------------------------------------
 */

static void DisplayHelp( void )
{

  fprintf( stderr,

  "Usage: kernelhdr inFile outFile\n"
  "\n"
  "      inFile: kernel image input file\n"
  "     outFile: kernel output file with header pre-pended\n"
  "\n" );


  exit(10);
}

/*---------------------------------------------------------------------
 * FUNCTION         : outputError
 *
 * DESCRIPTION      : This function reports output file error and exits
 *
 * PARAMETERS       : what  - description of what was being written
 *
 * RETURNS          : none
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static void outputError( char *what )
{
  fprintf( stderr, "Error while writing %s to output file\n", what );
  exit(1);
}

/*---------------------------------------------------------------------
 * FUNCTION         : bldKernelHdr
 *
 * DESCRIPTION      : This function builds the kernel image header
 *                    used by RMON for kernel boot validation.
 *
 * PARAMETERS       : hdr   - pointer of dest hdr struct
 *                    file  - file pointer of open kernel image file
 *
 * RETURNS          : none
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static void bldKernelHdr( DsFlashHdr *hdr, FILE *file )
{
  dword length, chksum;
  int b;

  /*
   * fill in kernel header with defaults
   */
  memset( hdr, 0, sizeof( *hdr ) );

  hdr->magic          = htonl( DS_FLASH_MAGIC );
  hdr->hdrVers        = htonl( 1 );
  hdr->flashOffset    = htonl( sizeof( *hdr ) );
  strcpy( hdr->signature, DS_FLASH_SIGNATURE );

  /*
   * compute length and checksum
   */
  length = 0;
  chksum = 0;
  while ( (b = fgetc( kernelFile )) != EOF )
  {
    length++;
    chksum += b;
  }
  hdr->flashLength = htonl( length );
  hdr->flashSum    = htonl( chksum );
}

/*---------------------------------------------------------------------
 * FUNCTION         : main()
 *
 * DESCRIPTION      : Main program function.
 *
 * PARAMETERS       : command line arguments of convention.
 *
 * RETURNS          : none
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

int main( int argc, char *argv[] )
{
  int argInx;
  int b, n;
  DsFlashHdr      kHdr;         /* kernel header */
  unsigned char   *bp;

  printf( "Kernel Image Header Utility : Version %s\n\n", VERSION );

  if ( argc != 3 )
  {
    DisplayHelp();
  }
  argInx = 1;
  kernelName = argv[argInx++];
  outName    = argv[argInx++];

  /*
   * attempt to open kernel input file
   */
  kernelFile = fopen( kernelName, "r" );
  if( ! kernelFile )
  {
    fprintf( stderr, "Error: Can't open kernel file: %s\n", kernelName );
    exit(1);
  }

  outFile = fopen( outName, "w" );
  if( ! outFile )
  {
    fprintf( stderr, "Error: Can't open output file: %s\n", outName );
    exit(1);
  }

  bldKernelHdr( & kHdr, kernelFile );

  if ( 0 != fseek( kernelFile, 0L, SEEK_SET ) )
  {
    fprintf( stderr, "Error while positioning kernel imput file\n" );
    exit(1);
  }

  /*
   * output kernel header
   */
  bp = (unsigned char *) & kHdr;
  for ( n = 0; n < sizeof( kHdr ); ++n )
  {
    if ( EOF == fputc( b = *bp++, outFile ) )
    {
      outputError( "KERNEL HDR" );
    }
  }

  /*
   * output kernel image
   */
  while ( (b = fgetc( kernelFile )) != EOF )
  {
    if ( EOF == fputc( b, outFile ) )
    {
      outputError( "KERNEL" );
    }
  }

  /*
   * close output file
   */
  if ( 0 != fclose( outFile ) )
  {
    fprintf( stderr, "Error while closing output file\n" );
    exit(1);
  }

  exit(0);    /* success */
}

/* end */

