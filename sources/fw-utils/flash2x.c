/*---------------------------------------------------------------------
 * Copyright 2002-2006, Cybex Computer Products Corporation
 *---------------------------------------------------------------------
 * FILE         : flash2x.c
 *
 * DESCRIPTION  : Utility program to construct 2x compatible flash
 *                download files for DS appliances.
 *
 *---------------------------------------------------------------------
 * AUTHOR       : David Stafford
 * DATE         : Jan 4-2002
 *
 *---------------------------------------------------------------------
 * HISTORY
 *
 * DATE         PROGRAMMER        MODIFICATIONS
 * Jan 4-2002   David Stafford    Initial version
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
#include <checksum.h>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

#define VERSION   "2.21"

#define DEBUG               /* debug this program */

/* Compiler initializes uninitialized global variables to zero */
static FILE *cmdFile;       /* command input file */
static char *cmdName;

static FILE *outFile;       /* flash output file */
static char *outName;

static FILE *kernelFile;    /* KERNEL input file */
static char *kernelName;

static FILE *dtbFile;       /* Device Tree input file */
static char *dtbName;

static FILE *cramfsFile;    /* CRAMFS input file */
static char *cramfsName;

static FILE *rmonFile;      /* RMON input file */
static char *rmonName;

static FILE *privkeyFile;   /* Private key file */
static char *privkeyName;
static RSA* rsa;
static long signaturePos;

#define MAX_PART_NUMBERS 20
static char *partNumbers[ MAX_PART_NUMBERS ];
static int partNumberCount = 0;

static char *fileVersion;   /* version number string for flash file */

#define MAX_OEM_CODES 20
static int oemCodes[ MAX_OEM_CODES ];
static int oemCodeCount = 0;

static int productFamily = -1;
static int hdrVers = 2;
static int checksumType = -1;   /* valid range 0-2 */

static char *countryCode;   /* ISO-3166-1 country code */
static char *languageCode;  /* ISO-639-2 language code */

static unsigned long flashLimit = 0;  /* non-zero indicates a range check is requested */
static unsigned long cramfsSize;
static unsigned long kernelSize;

#include "oem_product_table.h"

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
  int i;
  int oemCount = sizeof( oemTable ) / sizeof( struct OemTable );
  int prodCount = sizeof( prodTable ) / sizeof( struct ProductTable );

  fprintf( stderr, "Usage: flash2x cmdFile\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  cmdFile: File that contains control statements as follows:\n");
  fprintf( stderr, "           K Kernel image file name\n");
  fprintf( stderr, "           D Device Tree file name\n");
  fprintf( stderr, "           F CRAMFS file system image file name\n");
  fprintf( stderr, "           R Rmon (bootcode) image file name\n");
  fprintf( stderr, "           O Output file name\n");
  fprintf( stderr, "           V Flash part number and Version this flash file\n");
  fprintf( stderr, "           P Product Part Numbers compatible with this firmware\n");
  fprintf( stderr, "           OEM OEM code or OEM name\n");
  fprintf( stderr, "           FAMILY Product family code or name\n");
  fprintf( stderr, "           CCODE ISO-3166-1 country code\n");
  fprintf( stderr, "           LCODE ISO-639-2 language code\n");
  fprintf( stderr, "           CHECKSUM Checksum type (0-sum[default],1-CRC32,2-CRC16)\n");
  fprintf( stderr, "           PRIVKEY private key file name\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  Example file:\n");
  fprintf( stderr, "           K kernel.img\n");
  fprintf( stderr, "           F cramfs.img\n");
  fprintf( stderr, "           R rmon.img\n");
  fprintf( stderr, "           O flash_file.fl\n");
  fprintf( stderr, "           V 1.2.3.0\n");
  fprintf( stderr, "           P 520-244     (Multiple part numbers are ok)\n");
  fprintf( stderr, "           P 520-243\n");
  fprintf( stderr, "           OEM Avocent   (Multiple OEMs are ok)\n");
  fprintf( stderr, "           OEM Compaq\n");
  fprintf( stderr, "           FAMILY dsr2\n");
  fprintf( stderr, "           CCODE us\n");
  fprintf( stderr, "           LCODE eng\n");
  fprintf( stderr, "           CHECKSUM 1\n");
  fprintf( stderr, "           FLASH_LIMIT 700000\n");
  fprintf( stderr, "\n" );

  fprintf( stderr, "Valid OEM choices:\n" );
  for ( i = 0; i < oemCount; ++i )
  {
    fprintf( stderr, "  %3d or %s\n", oemTable[i].code, oemTable[i].name );
  }
  fprintf( stderr, "\n" );


  fprintf( stderr, "Valid Product/Family Choices:\n" );
  for ( i = 0; i < prodCount; ++i )
  {
    fprintf( stderr, "  %3d or %s\n", prodTable[i].code, prodTable[i].name );
  }
  fprintf( stderr, "\n" );

  exit(10);
}

/*---------------------------------------------------------------------
 * FUNCTION         : skipbl
 *
 * DESCRIPTION      : This function skips white space in a string
 *
 * PARAMETERS       : s - pointer of text string
 *
 * RETURNS          : pointer of first non-white text
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static char * skipbl( char * s )
{
  while ( *s == ' ' || *s == '\t' )
  {
    ++s;
  }
  return s;
}

/*---------------------------------------------------------------------
 * FUNCTION         : getOemCode
 *
 * DESCRIPTION      : This function decodes the OEM code from the command
 *                    line given.
 *
 * PARAMETERS       : cmdLine - pointer of buffer to decode
 *
 * RETURNS          : OEM number code
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static int getOemCode( char *cmdLine )
{
  int tableCount = sizeof( oemTable ) / sizeof( struct OemTable );
  int i, oemCode;
  char cmd[ 200 ];

  /*
   * search for symbolic OEM match
   */
  if ( 1 == sscanf( cmdLine, "%s", cmd ) )
  {
    for ( i = 0; i < tableCount; ++i )
    {
      if ( strcasecmp( cmd, oemTable[i].name ) == 0 )
      {
        return oemTable[i].code;
      }
    }
  }
  /*
   * search for numeric OEM match
   */
  if ( 1 != sscanf( cmdLine, "%x", (unsigned int *) ( & oemCode ) ) )
  {
    if ( 1 != sscanf( cmdLine, "%d", & oemCode ) )
    {
      fprintf( stderr, "Error: Bad OEM code [%s]\n", cmdLine );
      exit(1);
    }
  }
  for ( i = 0; i < tableCount; ++i )
  {
    if ( oemCode == oemTable[i].code )
    {
      return oemTable[i].code;
    }
  }
  fprintf( stderr, "Error: Bad OEM code [%s]\n", cmdLine );
  exit(1);
  return 0;
}

/*---------------------------------------------------------------------
 * FUNCTION         : getProductFamily
 *
 * DESCRIPTION      : This function decodes the Product Family code from the command
 *                    line given.
 *
 * PARAMETERS       : cmdLine - pointer of buffer to decode
 *
 * RETURNS          : Product family code
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static int getProductFamily( char *cmdLine )
{
  int tableCount = sizeof( prodTable ) / sizeof( struct ProductTable );
  int i, prodCode;
  char cmd[ 200 ];

  /*
   * search for symbolic product match
   */
  if ( 1 == sscanf( cmdLine, "%s", cmd ) )
  {
    for ( i = 0; i < tableCount; ++i )
    {
      if ( strcasecmp( cmd, prodTable[i].name ) == 0)
      {
        return prodTable[i].code;
      }
    }
  }
  /*
   * search for numeric product match
   */
  if ( 1 != sscanf( cmdLine, "%x", (unsigned int *)( & prodCode ) ) )
  {
    if ( 1 != sscanf( cmdLine, "%d", & prodCode ) )
    {
      fprintf( stderr, "Error: Bad product family code [%s]\n", cmdLine );
      exit(1);
    }
  }
  for ( i = 0; i < tableCount; ++i )
  {
    if ( prodCode == prodTable[i].code )
    {
      return prodTable[i].code;
    }
  }
  fprintf( stderr, "Error: Bad product family code [%s]\n", cmdLine );
  exit(1);
  return 0;
}
/*---------------------------------------------------------------------
 * FUNCTION         : getFileHeaderVersion
 *
 * DESCRIPTION      : This function decodes the File Header Version from
 *                    the command line given to be output into the new file.
 *
 * PARAMETERS       : cmdLine - pointer of buffer to decode
 *
 * RETURNS          : binary version of File Header Fersion string
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static int getFileHeaderVersion( char *cmdLine )
{
  int hdrVers;
  sscanf(cmdLine, "%d", &hdrVers );
  return hdrVers;
}

/*---------------------------------------------------------------------
 * FUNCTION         : getChecksumType
 *
 * DESCRIPTION      : This function decodes the checksum type from
 *                    the command line given to be output into the new file.
 *
 * PARAMETERS       : cmdLine - pointer of buffer to decode
 *
 * RETURNS          : checksum type
 *                    0: 32-bit sum of bytes (default)
 *                    1: CRC32
 *                    2: CRC16
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static int getChecksumType ( char *cmdLine )
{
  int type;
  sscanf(cmdLine, "%d", &type );
  return type;
}

/*---------------------------------------------------------------------
 * FUNCTION         : parseCmdFile
 *
 * DESCRIPTION      : This function parses the command file
 *
 * PARAMETERS       : none
 *
 * RETURNS          :
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static void parseCmdFile( void )
{
  char cmd[ 200 ], cmdLine[ 200 ];
  char fileName[ 200 ];
  char text[ 200 ];
  char *cp, *ln;
  int pn1, pn2;
  int maxlen;

  /*
   * parse command file for required components
   */
  while ( fgets( cmdLine, sizeof(cmdLine), cmdFile ) )
  {
    if ( 1 != sscanf( cmdLine, "%s", cmd ) )
    {
      continue;
    }

    if ( 0==strcasecmp( "K", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+1, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad KERNEL file name\n" );
        exit(1);
      }
      kernelName = strdup( fileName );
#ifdef DEBUG
      printf( "Kernel name : [%s]\n", kernelName );
#endif
    }
    else if ( 0==strcasecmp( "D", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+1, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad Devire Tree file name\n" );
        exit(1);
      }
      dtbName = strdup( fileName );
#ifdef DEBUG
      printf( "Cramfs name : [%s]\n", cramfsName );
#endif
    }
    else if ( 0==strcasecmp( "F", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+1, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad CRAMFS file name\n" );
        exit(1);
      }
      cramfsName = strdup( fileName );
#ifdef DEBUG
      printf( "Cramfs name : [%s]\n", cramfsName );
#endif
    }
    else if ( 0==strcasecmp( "R", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+1, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad RMON file name\n" );
        exit(1);
      }
      rmonName = strdup( fileName );
#ifdef DEBUG
      printf( "Rmon name   : [%s]\n", rmonName );
#endif
    }
    else if ( 0==strcasecmp( "O", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+1, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad output file name\n" );
        exit(1);
      }
      outName = strdup( fileName );
#ifdef DEBUG
      printf( "Output name : [%s]\n", outName );
#endif
    }
    else if ( 0==strcasecmp( "PRIVKEY", cmd ) )
    {
      if ( 1 != sscanf( cmdLine+7, " %s", fileName ) )
      {
        fprintf( stderr, "Error: Bad private key file name\n" );
        exit(1);
      }
      privkeyName = strdup( fileName );
#ifdef DEBUG
      printf( "Private key : [%s]\n", privkeyName );
#endif
    }
    else if ( 0==strcasecmp( "V", cmd ) )
    {
      cp = skipbl( cmdLine+1 );
      ln = strchr( cp, '\n' );
      if( ln ) *ln = 0;

      if ( strlen( cp ) )
      {
        strcpy( text, cp );
      }
      else
      {
        fprintf( stderr, "Error: Bad version number\n" );
        exit(1);
      }
      /*
       * limit version string to size of header's field
       */
      maxlen = sizeof( ((DsFlashFileVersion*)0)->version )-1;
      if ( strlen( text ) > maxlen )
      {
        text[ maxlen ] = 0;
      }
      fileVersion = strdup( text );
#ifdef DEBUG
      printf( "File version: [%s]\n", fileVersion );
#endif
    }
    else if ( 0==strcasecmp( "P", cmd ) )
    {
      if ( partNumberCount >= MAX_PART_NUMBERS )
      {
        fprintf( stderr, "Error: Too many part numbers\n" );
        exit(1);
      }
      if ( 2 != sscanf( cmdLine+1, " %d-%d", & pn1, & pn2 ) )
      {
        fprintf( stderr, "Error: Bad part number\n" );
        exit(1);
      }
      sprintf( text, "%03d-%03d", pn1, pn2 );
      partNumbers[ partNumberCount ] = strdup( text );
#ifdef DEBUG
      printf( "Part number : [%s]\n", partNumbers[ partNumberCount ] );
#endif
      partNumberCount++;
    }
    else if ( 0==strcasecmp( "OEM", cmd ) )
    {
      if ( oemCodeCount >= MAX_OEM_CODES )
      {
        fprintf( stderr, "Error: Too many OEM codes\n" );
        exit(1);
      }
      oemCodes[ oemCodeCount ] = getOemCode( cmdLine+3 );
#ifdef DEBUG
      printf( "OEM code    : [%d]\n", oemCodes[ oemCodeCount ] );
#endif
      oemCodeCount++;
    }
    else if ( 0==strcasecmp( "FAMILY", cmd ) )
    {
      productFamily = getProductFamily( cmdLine+6 );
#ifdef DEBUG
      printf( "Family code : [%d]\n", productFamily );
#endif
    }
    else if ( 0==strcasecmp( "CCODE", cmd ) )
      {
        if ( (1 != sscanf( cmdLine+5, " %s", fileName )) ||
             (2 != strlen(fileName)))
          {
            fprintf( stderr, "Error: Bad country code\n" );
            exit(1);
          }
        countryCode = strdup( fileName );

#ifdef DEBUG
        printf( "Country code : [%s]\n", countryCode );
#endif
      }
    else if ( 0==strcasecmp( "LCODE", cmd ) )
      {
        if ( (1 != sscanf( cmdLine+5, " %s", fileName )) ||
             (3 != strlen(fileName)))
          {
            fprintf( stderr, "Error: Bad language code\n" );
            exit(1);
          }
        languageCode = strdup( fileName );

#ifdef DEBUG
        printf( "Language code : [%s]\n", languageCode );
#endif
      }
    else if (0==strcasecmp( "FILEVERS", cmd) )
    {
      hdrVers = getFileHeaderVersion( cmdLine + 8 );
#ifdef DEBUG
      printf( "File Hdr Ver: [%d]\n", hdrVers );
#endif
    }
    else if (0==strcasecmp( "CHECKSUM", cmd) )
    {
      checksumType = getChecksumType ( cmdLine+8 );
      if ( (0 > checksumType) || (2 < checksumType) )
      {
        fprintf( stderr, "Error: Bad checksum type [%d]\n", checksumType );
        exit(1);
      }
#ifdef DEBUG
      printf( "Checksum Type: [%d]\n", checksumType );
#endif
    }
    else if (0==strcasecmp( "FLASH_LIMIT", cmd) )
    {
      if ( 1 != sscanf( cmdLine+11, "%lx", & flashLimit ) )
      {
        fprintf( stderr, "Error: Bad flash size limit\n" );
        exit(1);
      }
#ifdef DEBUG
      printf( "Flash limit : [%08x]\n", (unsigned int)flashLimit );
#endif
    }
    else
    {
      fprintf( stderr, "Error: Bad command: [%s]\n", cmd );
      exit(1);
    }
  }

  /*
   * verify required components have been found in command file
   */
  if ( ! kernelName )
  {
    fprintf( stderr, "Error: No kernel file specified\n" );
    exit(1);
  }

  if ( ! cramfsName )
  {
    fprintf( stderr, "Error: No CRAMFS file specified\n" );
    exit(1);
  }

  if ( ! rmonName )
  {
    fprintf( stderr, "Error: No RMON file specified\n" );
    exit(1);
  }

  if ( ! outName )
  {
    fprintf( stderr, "Error: No output file specified\n" );
    exit(1);
  }

  if( ! fileVersion )
  {
    fprintf( stderr, "Error: No flash file version specified\n" );
    exit(1);
  }

  if( partNumberCount == 0 )
  {
    fprintf( stderr, "Error: No part numbers specified\n" );
    exit(1);
  }

  if( ! countryCode )
  {
    fprintf( stderr, "Error: No country code specified\n" );
    exit(1);
  }

  if( ! languageCode )
  {
    fprintf( stderr, "Error: No language code specified\n" );
    exit(1);
  }
}


/*---------------------------------------------------------------------
 * FUNCTION         : defaultHdr
 *
 * DESCRIPTION      : This function fills in default values for
 *                    flash file header.
 *
 * PARAMETERS       : hdr   - pointer of header struct
 *
 * RETURNS          : none
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static void defaultHdr( DsFlashFileHdr1 *hdr )
{
  time_t nowTime;       /* epoch time */
  struct tm borkenTime; /* broken down time */

  /*
   * init struct to zeroes
   */
  memset( hdr, 0, sizeof( *hdr ) );


  hdr->image_size = 0xFFFFFFFF;

  /*
   * get time and date into hdr struct
   */
  nowTime = time( 0 );
  borkenTime = * localtime( & nowTime );
  strftime( (char *)(hdr->image_create_date), sizeof(hdr->image_create_date), "%m/%d/%y", & borkenTime );
  strftime( (char *)(hdr->image_create_time), sizeof(hdr->image_create_time), "%H:%M:%S", & borkenTime );

#ifdef DEBUG
  printf( "image_create_date : [%s]\n", hdr->image_create_date );
  printf( "image_create_time : [%s]\n", hdr->image_create_time );
#endif

  hdr->image_type           = 'A';  /* app type */
  hdr->image_appliance_type =  1 ;  /* DS type */

  hdr->hdrMagic  = htonl( DS_FLASH_HDR1_MAGIC );
  hdr->hdrVers   = htonl( hdrVers );
  hdr->hdrLength = htonl( sizeof( DsFlashFileHdr1 ) );

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
 * FUNCTION         : fileAlign4
 *
 * DESCRIPTION      : This function forces a writable file to a 4 byte
 *                    alignment. Null bytes are written to align as
 *                    needed.
 *
 * PARAMETERS       : file    - file stream pointer
 *
 * RETURNS          : resulting file offset
 *
 * NOTES            :
 *
 *---------------------------------------------------------------------
 */

static long fileAlign4( FILE *file )
{
  long offset, diff;

  offset = ftell( file );
  diff   = offset % 4;

  if ( diff != 0 )
  {
    diff = 4 - diff;

#ifdef DEBUG
    printf( "fileAlign4() offset = %d, diff = %d\n", (int) offset, (int) diff );
#endif

    while ( diff-- )
    {
      if ( EOF == fputc( 0x00, file ) )
      {
        outputError( "alignment bytes" );
      }
      offset++;
    }
  }
  return offset;
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
  dword offset, length, chksum;  /* file component offset and byte count */
  int b, n;
  DsFlashFileHdr1 hdr;
  DsHwPartNumber  pn;
  DsFlashHdr      kHdr;         /* kernel header */
  unsigned char   *bp;

  OptRecHdr rec;
  unsigned short recParm;
  dword optRecLength;
  int actualChksumType;         /* actual chksum type to use */

  printf( "DS1800/DSR 2.x Flash File Builder Utility : Version %s\n\n", VERSION );

  buildCRCTbl();

  if ( argc != 2 )
  {
    DisplayHelp();
  }
  argInx = 1;
  cmdName = argv[argInx++];

  /*
   * attempt to open command input file
   */
  cmdFile = fopen( cmdName, "r" );
  if ( ! cmdFile )
  {
    fprintf( stderr, "\nCan't open command file: [%s]\n", cmdName );
    exit(1);
  }

  /*
   * parse the command file
   */
  parseCmdFile();

  /*
   * open the assorted files
   */
  kernelFile = fopen( kernelName, "r" );
  if( ! kernelFile )
  {
    fprintf( stderr, "Error: Can't open kernel file: %s\n", kernelName );
    exit(1);
  }

  if (dtbName) {
    dtbFile = fopen( dtbName, "r" );
    if( ! dtbFile )
    {
      fprintf( stderr, "Error: Can't open device tree file: %s\n", dtbName );
      exit(1);
    }
  }
  cramfsFile = fopen( cramfsName, "r" );
  if( ! cramfsFile )
  {
    fprintf( stderr, "Error: Can't open CRAMFS file: %s\n", cramfsName );
    exit(1);
  }

  rmonFile = fopen( rmonName, "r" );
  if( ! rmonFile )
  {
    fprintf( stderr, "Error: Can't open RMON file: %s\n", rmonName );
    exit(1);
  }

  outFile = fopen( outName, "w+" );
  if( ! outFile )
  {
    fprintf( stderr, "Error: Can't open output file: %s\n", outName );
    exit(1);
  }

  if (privkeyName) {
    privkeyFile = fopen( privkeyName, "r" );
    if( ! privkeyFile )
    {
      fprintf( stderr, "Error: Can't open private key file: %s\n", privkeyName );
      exit(1);
    }
    rsa = PEM_read_RSAPrivateKey( privkeyFile, NULL, NULL, NULL );
    fclose( privkeyFile );
    if ( ! rsa) {
      fprintf( stderr, "Error: Can't read private key\n");
      exit(1);
    }
  }

  /*
   * Fill in some default values
   */
  defaultHdr( & hdr );

  /*
   * Copy in country and language codes
   */
  memcpy(hdr.image_country, countryCode, sizeof(hdr.image_country));
  memcpy(hdr.image_language, languageCode, sizeof(hdr.image_language));

  /*
   * write partially completed header to output to use a placeholder
   */
  if ( 1 != fwrite( & hdr, sizeof(hdr), 1, outFile ) )
  {
    outputError( "partial header" );
  }


  /*
   * process KERNEL image file
   */
  offset = fileAlign4( outFile );
  length = 0;
  chksum = 0;

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
    if ( EOF == fputc( b = (int) (*bp++), outFile ) )
    {
      outputError( "KERNEL HDR" );
    }
    length++;
  }

  if (checksumType == -1 )
  {
    actualChksumType = 0;
  }
  else
  {
    actualChksumType = checksumType;
  }
  chksum = computeBufferChecksum( actualChksumType, chksum, (char *)&kHdr, sizeof(kHdr) );

  /*
   * output kernel image
   */
  while ( (b = fgetc( kernelFile )) != EOF )
  {
    if ( EOF == fputc( b, outFile ) )
    {
      outputError( "KERNEL" );
    }
    length++;
  }

  chksum = computeFileChecksum( actualChksumType, chksum, kernelFile );

  if (dtbFile) {
    while ( (b = fgetc( dtbFile )) != EOF )
    {
      if ( EOF == fputc( b, outFile ) )
      {
        outputError( "Device Tree" );
      }
      length++;
    }
    chksum = computeFileChecksum( actualChksumType, chksum, dtbFile );
  }

  hdr.zImageOffset = htonl( offset );
  hdr.zImageLength = htonl( length );
  hdr.zImageChkSum = htonl( chksum );
#ifdef DEBUG
  printf( "KERNEL offset %d\n", (int) offset );
  printf( "KERNEL length %d\n", (int) length );
  printf( "KERNEL chksum 0x%08X\n", (unsigned int) chksum );
#endif
  kernelSize = length;

  /*
   * process CRAMFS image file
   */
  offset = fileAlign4( outFile );
  length = 0;
  chksum = 0;

  while ( (b = fgetc( cramfsFile )) != EOF )
  {
    if ( EOF == fputc( b, outFile ) )
    {
      outputError( "CRAMFS" );
    }
    length++;
  }

  chksum = computeFileChecksum( actualChksumType, chksum, cramfsFile );

  hdr.cramfsOffset = htonl( offset );
  hdr.cramfsLength = htonl( length );
  hdr.cramfsChkSum = htonl( chksum );
#ifdef DEBUG
  printf( "CRAMFS offset %d\n", (int) offset );
  printf( "CRAMFS length %d\n", (int) length );
  printf( "CRAMFS chksum 0x%08X\n", (unsigned int) chksum );
#endif
  cramfsSize = length;

  /*
   * process RMON image file
   */
  offset = fileAlign4( outFile );
  length = 0;
  chksum = 0;

  while ( (b = fgetc( rmonFile )) != EOF )
  {
    if ( EOF == fputc( b, outFile ) )
    {
      outputError( "RMON" );
    }
    length++;
  }

  chksum = computeFileChecksum( actualChksumType, chksum, rmonFile );

  hdr.rmonOffset = htonl( offset );
  hdr.rmonLength = htonl( length );
  hdr.rmonChkSum = htonl( chksum );
#ifdef DEBUG
  printf( "RMON offset %d\n", (int) offset );
  printf( "RMON length %d\n", (int) length );
  printf( "RMON chksum 0x%08X\n", (unsigned int) chksum );
#endif

  /*
   * output PART Numbers
   */
  offset = fileAlign4( outFile );

  for ( n = 0; n < partNumberCount; ++n )
  {
    memset( & pn, 0, sizeof( pn ) );
    strncpy( pn.partNumber, partNumbers[ n ], sizeof( pn.partNumber )-1 );
    if ( 1 != fwrite( & pn, sizeof( pn ), 1, outFile ) )
    {
      outputError( "Part Numbers" );
    }
  }
  hdr.partNumOffset = htonl( offset );
  hdr.partNumCount  = htonl( partNumberCount );
#ifdef DEBUG
  printf( "PART offset %d\n", (int) offset );
  printf( "PART count  %d\n", (int) partNumberCount );
#endif

  /*
   * output Option Record section
   */
  offset = fileAlign4( outFile );

  /*
   * output OEM code Option records
   */
  for ( n = 0; n < oemCodeCount; ++n )
  {
    rec.recType   = htons( OPTREC_TYPE_OEM );
    rec.recLength = htons( sizeof( recParm ) );
    recParm       = htons( oemCodes[ n ] );
    if ( 1 != fwrite( & rec, sizeof( rec ), 1, outFile ) )
    {
      outputError( "OEM Codes" );
    }
    if ( 1 != fwrite( & recParm, sizeof( recParm ), 1, outFile ) )
    {
      outputError( "OEM Codes" );
    }
  }

  /*
   * output Product Family code Option record
   */
  if ( productFamily != -1 )
  {
    rec.recType   = htons( OPTREC_TYPE_FAMILY );
    rec.recLength = htons( sizeof( recParm ) );
    recParm       = htons( productFamily );
    if ( 1 != fwrite( & rec, sizeof( rec ), 1, outFile ) )
    {
      outputError( "Product Family Code" );
    }
    if ( 1 != fwrite( & recParm, sizeof( recParm ), 1, outFile ) )
    {
      outputError( "Product Family Code" );
    }
  }

  /* output Checksum Type Option record */
  /* If checksumType == -1, it has not be set by the user. In that case, don't add
     it to the file and let the appliance use the default checksum type */
  if ( checksumType != -1 )
  {
    rec.recType   = htons( OPTREC_TYPE_CHECKSUM );
    rec.recLength = htons( sizeof( recParm ) );
    recParm       = htons( checksumType );
    if ( 1 != fwrite( & rec, sizeof( rec ), 1, outFile ) )
    {
      outputError( "Checksum Type" );
    }
    if ( 1 != fwrite( & recParm, sizeof( recParm ), 1, outFile ) )
    {
      outputError( "Checksum Type" );
    }
  }

  if (rsa) {
     char buf[SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE];
     rec.recType   = htons( OPTREC_TYPE_SIGNATURE );
     rec.recLength = htons( sizeof( recParm) +
         SIGNATURE_METHOD_SHA1_RSA_KEYSIZE +
         SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE);
     recParm       = htons( SIGNATURE_METHOD_SHA1_RSA );
     if ( 1 != fwrite( & rec, sizeof( rec ), 1, outFile ) )
     {
       outputError( "Signature" );
     }
     if ( 1 != fwrite( & recParm, sizeof( recParm ), 1, outFile ) )
     {
        outputError( "Signature" );
     }
     if (i2d_RSA_PUBKEY_fp( outFile, rsa ) <= 0)
     {
        outputError( "Signature" );
     }
     signaturePos = ftell(outFile);
     memset(buf, 0, SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE);
     if ( SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE !=
	fwrite( buf, 1, SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE, outFile ) )
     {
        outputError( "Signature" );
     }
  }

  /*
   * output END Option Record
   */
  rec.recType   = htons( OPTREC_TYPE_END );
  rec.recLength = htons( 0 );
  if ( 1 != fwrite( & rec, sizeof( rec ), 1, outFile ) )
  {
    outputError( "END Record" );
  }

  optRecLength = ftell( outFile ) - offset;

  hdr.optRecOffset  = htonl( offset );
  hdr.optRecLength  = htonl( optRecLength );
#ifdef DEBUG
  printf( "OPTREC offset %d\n", (int) offset );
  printf( "OPTREC length %d\n", (int) optRecLength );
#endif

  /*
   * reposition output file to re-write updated header
   */
  if ( 0 != fseek( outFile, 0L, SEEK_SET ) )    /* goto start of file */
  {
    fprintf( stderr, "Error while positioning output file for completed header\n" );
    exit(1);
  }

  /*
   * copy flash file version into header
   */
  strncpy( hdr.flashFileVersion.version, fileVersion, sizeof( hdr.flashFileVersion.version )-1 );

  /*
   * write completed header to output file
   */
  if ( 1 != fwrite( & hdr, sizeof(hdr), 1, outFile ) )
  {
    outputError( "completed header" );
  }

  /*
   * calculate and write signature
   */
  if (rsa)
  {
     SHA_CTX ctx;
     unsigned int len = SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE;
     unsigned char hash[ SHA_DIGEST_LENGTH ];
     unsigned char buf[SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE];
     unsigned char tmp[1024];

     fseek( outFile, 0, SEEK_SET);
     SHA1_Init( &ctx );
     for ( ;; )
     {
        int cnt = fread( tmp, 1, 1024, outFile);
        if (cnt == 0) break;
        SHA1_Update( &ctx, tmp, cnt );
     }
     SHA1_Final( hash, &ctx );
     if (RSA_sign( NID_sha1, hash, SHA_DIGEST_LENGTH, buf, &len, rsa ) == 0)
     {
        outputError( "RSA signature" );
     }
     fseek( outFile, signaturePos, SEEK_SET);
     fwrite( buf, 1, SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE, outFile );
  }


  /*
   * close output file
   */
  if ( 0 != fclose( outFile ) )
  {
    fprintf( stderr, "Error while closing output file\n" );
    exit(1);
  }

  /*
   * perform optional range check on kernel and cramfs combined size
   */
  if ( flashLimit != 0 )
  {
    unsigned long totalSize;
    totalSize = ((cramfsSize + 1023) / 1024) * 1024; /* round to next 1K multiple */
    totalSize += kernelSize;
    printf( "Flash space available for CRAMFS+Kernel is %08X\n", (unsigned int) flashLimit );
    printf( "      Flash space used by CRAMFS+Kernel is %08X\n", (unsigned int) totalSize );
    if ( totalSize <= flashLimit )
    {
      unsigned long unusedSize;
      unusedSize = flashLimit - totalSize;
      printf( "                  Flash space remaining is %08X (%d)\n", (unsigned int) unusedSize, (int) unusedSize );
    }
    else
    {
      printf( "Error: Flash memory space exceeded\n" );
      exit(1);
    }
  }

  exit(0);    /* success */
}

/* end */

