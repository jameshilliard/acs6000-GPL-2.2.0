//---------------------------------------------------------------------
// Copyright 2002-2003, Cybex Computer Products Corporation
//---------------------------------------------------------------------
// FILE         : flashhdr.h
//
// DESCRIPTION  : Defines flash file header used in validation of flash
//
//---------------------------------------------------------------------
// AUTHOR       : David Stafford
// DATE         : Oct 3 2000
//
//---------------------------------------------------------------------
// HISTORY
//
// DATE         PROGRAMMER        MODIFICATIONS
// Oct 3 2000   David Stafford    Initial version
// Jan 5 2002   David Stafford    Revised for use with ViewPlus
//
//---------------------------------------------------------------------

#ifndef __FLASHHDR_H
#define __FLASHHDR_H

#include "oem_code.h"
#include "prodcode.h"

/* ===============================================================*/
/*                 OLD HEADER FORMAT before ViewPlus              */
/*                 OLD HEADER FORMAT before ViewPlus              */
/*                 OLD HEADER FORMAT before ViewPlus              */
/* ===============================================================*/


/*
 * Value that must be compared in the 'signature' member
 * of FlashFileHdr struct
 */

#define FLASH_SIGNATURE "DS1800 Vers "  /* exactly 12 bytes in length */

/*
 * This struct must be 32 bytes in size. It is found at the
 * beginning of the DS1800 flash file and in the upgrade bank of
 * flash memory.
 */
typedef struct
{
  /*
   * Use the 'signature' member to validate file type.
   * The version value is for visual perusal only. It should
   * match the DS1800 application version.
   */
  char signature[ 12 ];         /* signature string (use memcmp() to compare this) */
  char version  [ 12 ];         /* "aa.bb.cc.dd\0" null terminated vers string     */

  /*
   * These values are in big endian format. This is native to PowerPc and
   * no special handling is required by RMON or DS1800 app code.
   */
  unsigned long fileLength;     /* number of flash bytes in flash file */
  unsigned long fileSum;        /* sum of bytes in flash file          */

} FlashFileHdr;


/* ===============================================================*/
/*               NEW HEADER FORMAT used with ViewPlus             */
/*               NEW HEADER FORMAT used with ViewPlus             */
/*               NEW HEADER FORMAT used with ViewPlus             */
/* ===============================================================*/


/*
 * This struct is used by RMON to locate a valid
 * flash image for booting. RMON will first use the flash boot
 * address parameters to find a boot image. If this does not yield a
 * valid boot image, then the first kernel image that is found
 * with this header will be used.
 */

#define DS_FLASH_MAGIC     0xFEEDBEEF
#define DS_FLASH_SIGNATURE "Avocent DS Appliance"

typedef struct
{
  unsigned long magic;          /* contains DS_FLASH_MAGIC constant */
  unsigned long hdrVers;        /* this is version 1 */
  unsigned long flashOffset;    /* offset to flash from start of hdr */
  unsigned long flashLength;    /* number of flash bytes */
  unsigned long flashSum;       /* sum of flash bytes */
  char signature[ 32 ];         /* contains DS_FLASH_SIGNATURE string */
  char unused[ 12 ];            /* makes struct 64 bytes */

} DsFlashHdr;



/*
 * This struct is used to contain product firmware part number strings. The
 * DS applinace flash download file has a part number like this.
 */
typedef struct DsFwPartNumber
{
  char partNumber[ 12 ];    /* ascii part number string formated as xxx-xxx-xxx  */
                            /* string is null terminated */

        /* NOTE: This struct is sized to be a multiple of 4   */
        /*       bytes so that an array of these will work    */
        /*       on the PowerPc.                              */

} DsFwPartNumber ;


/*
 * This struct is used to contain product part number strings.
 * One component of the DS appliance flash download file contains a list of
 * these product part numbers. This list specifies the products into which
 * this flash can correctly be installed.
 */
typedef struct DsHwPartNumber
{
  char partNumber[ 8 ];     /* ascii part number string formated as xxx-xxx */
                            /* string is null terminated */

        /* NOTE: This struct is sized to be a multiple of 4   */
        /*       bytes so that an array of these will work    */
        /*       on the PowerPc.                              */

} DsHwPartNumber ;

/*
 * This struct is used to contain product flash file version number.
 */
typedef struct DsFlashFileVersion
{
  char version[ 32 ];       /* ascii version number string    */
                            /* string is null terminated      */

        /* NOTE: This struct is sized to be a multiple of 4   */
        /*       bytes so that an array of these will work    */
        /*       on the PowerPc.                              */

} DsFlashFileVersion ;


/*
 * This struct is the Flash File header used on DS appliances flash firmware
 * files.
 */
typedef struct DsFlashFileHdr1
{

// =============================================================
// The 1st portion of this struct is used to indentify firmware  files used
// by DS and CPS appliances and so forth. The ViewPlus client can read
// these headers to distinguish appliance firmware files. This portion of
// this struct evolved from one already in use by the CPS. The
// "image_scratch" member has been overlaid for use as a Product Ident so
// ViewPlus can tell a DS from a CPS.
// =============================================================

/*000*/ unsigned long     image_size;             // Load Image's Size (NOT including this Header)
                                                  // 0xFFFFFFFF = Region not loaded
                                                  // Not used by DSR; is always = 0xFFFFFFFF

/*004*/ unsigned short    image_crc;              // Load Image's CRC (i.e. CRC of length "size" - Header
                                                  // NOT included in CRC calculation)
                                                  // Not used by DSR; is always 0x0000

//Following fields are used when Region Image is created by DOS Utility
/*006*/ unsigned char     image_create_date[10];  // Date Region Image was created. ASCII MM/D/YY.
                                                  // null terminated & padded to even boundry

/*010*/ unsigned char     image_create_time[10];  // Time Region Image was created. ASCII HH:MM:SS,
                                                  // null terminated & padded to even boundry

/*01A*/ unsigned char     image_rev_a[8];         // Revision of Image. ASCII RR.MM,
                                                  // null terminated & padded to even boundry
                                                  // Not used by DSR; is always 0

/*022*/ unsigned char     image_rev_b[8];         // If needed, 2nd Revision of Image. ASCII RR.MM,
                                                  // null terminated & padded to even boundry
                                                  // Not used by DSR; is always 0

/*02A*/ unsigned char     image_file_name[14];    // File name of Load Image (DOS file name)
                                                  // null terminated & padded to even boundry
                                                  // Not used by DSR; is always 0

/*038*/ unsigned char     image_type;             // 'A' means APP region
                                                  // 'B' means BOOT region
                                                  // 'C' means CONFIG region
                                                  // Not used by DSR; is always 'A'

/*039*/ unsigned char     image_compress_flag;    // 0x00 means not compressed
                                                  // 'Y' means compressed lh6 file
                                                  // Not used by DSR; is always 0

/*03A*/ unsigned char     image_appliance_type;   // Used to identify CPS or DS appliance
                                                  //     0 - means CPS
                                                  //     1 - means DS appliance
                                                  // Only used by DS appliance

/*03B*/ unsigned char     image_country[2];       // Used to hold two character ISO-3166-1 country code
                                                  // Not NULL terminated

/*03D*/ unsigned char     image_language[3];      // Used to hold three character ISO-639-2 language code
                                                  // Not NULL terminated

// =============================================================
// The 2nd portion of this struct is used only by the DS appliance. It
// is used to identify several flash download components that are programmed
// seperately by the version 2.X appliance.
// =============================================================

#define DS_FLASH_HDR1_MAGIC 0xBEEF1111

  // the following long values are always in network order (Big Endian)

  unsigned long hdrMagic;       /* contains DS_FLASH_HDR1_MAGIC constant */
  unsigned long hdrVers;        /* this is header version 2 */
  unsigned long hdrLength;      /* length of this entire header */

  unsigned long zImageOffset;   /* file offset to zImage */
  unsigned long zImageLength;   /* length in bytes of zImage */
  unsigned long zImageChkSum;   /* sum of bytes in zImage */

  unsigned long cramfsOffset;   /* file offset to CRAMFS */
  unsigned long cramfsLength;   /* length in bytes of CRAMFS */
  unsigned long cramfsChkSum;   /* sum of bytes in CRAMFS */

  unsigned long rmonOffset;     /* file offset to RMON */
  unsigned long rmonLength;     /* length in bytes of RMON */
  unsigned long rmonChkSum;     /* sum of bytes in RMON */

  unsigned long partNumOffset;  /* file offset to compatible product part numbers */
  unsigned long partNumCount;   /* number of part numbers listed */
                /* part numbers are array of typedef DsHwPartNumber */

  DsFlashFileVersion flashFileVersion;  /* version number of this file */

  /* end of struct members associated with version 1 of this header */

  unsigned long optRecOffset;   /* file offset to optional records */
  unsigned long optRecLength;   /* length in bytes of optional records */

  /* end of struct members associated with version 2 of this header */


} DsFlashFileHdr1;

/*
 * optional records all have the following header
 */
typedef struct OptRecHdr
{
  unsigned short recType;
  unsigned short recLength;
} OptRecHdr;

/*
 * RECORD TYPES for Flash file header optional records
 */
#define OPTREC_TYPE_OEM       1
#define OPTREC_TYPE_FAMILY    2
#define OPTREC_TYPE_CHECKSUM  3
#define OPTREC_TYPE_SIGNATURE 4
#define OPTREC_TYPE_END       0xFFFF

/*
 * SIGNATURE METHODS
 */
#define SIGNATURE_METHOD_SHA1_RSA	0

#define SIGNATURE_METHOD_SHA1_RSA_KEYSIZE	162
#define SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE	128

/*
 * Values for ( OEM RECORD TYPE )
 *  Note: These values match Steve Anderson's firmware evolution control doc
 */
#define OPTREC_OEM_AVOCENT    OEM_CODE_AVOCENT
#define OPTREC_OEM_COMPAQ     OEM_CODE_COMPAQ   /* Used by Main Board to indicate HP/Compaq */
#define OPTREC_OEM_DELL       OEM_CODE_DELL
#define OPTREC_OEM_HP         OEM_CODE_HP       /* Used by Dmodule to indicate HP/Compaq    */
#define OPTREC_OEM_IBM        OEM_CODE_IBM
#define OPTREC_OEM_SMS        OEM_CODE_SMS      /* Fujitsu - Siemens                        */
#define OPTREC_OEM_WL         OEM_CODE_WL       /* Wright Line                              */
#define OPTREC_OEM_BLACKBOX   OEM_CODE_BLACKBOX
#define OPTREC_OEM_FCL   	OEM_CODE_FCL
#define OPTREC_OEM_TESTRACK   OEM_CODE_TESTRACK

/*
 * Values for ( Product Family RECORD TYPE )
 */
#define OPTREC_PRODUCT_TYPE_DS1800         PRODUCT_TYPE_DS1800        /* DS1800                             */
#define OPTREC_PRODUCT_TYPE_DSR            PRODUCT_TYPE_DSR           /* DSR1161, DSR2161, and DSR4160      */
#define OPTREC_PRODUCT_TYPE_AVR            PRODUCT_TYPE_AVR           /* A1000R, A2000R, and OEM variations */
#define OPTREC_PRODUCT_TYPE_DSR800         PRODUCT_TYPE_DSR800        /* DSR800                             */
#define OPTREC_PRODUCT_TYPE_DSR3           PRODUCT_TYPE_DSR3          /* DSR1010, DSR2010, DSR4010          */
#define OPTREC_PRODUCT_TYPE_BVIEW          PRODUCT_TYPE_BVIEW         /* DSR1021, DSR1022                   */
#define OPTREC_PRODUCT_TYPE_TWIG           PRODUCT_TYPE_TWIG          /* DSR1024                            */
#define OPTREC_PRODUCT_TYPE_ATLANTIS_A     PRODUCT_TYPE_ATLANTIS_A    /* Atlantis Analog                    */
#define OPTREC_PRODUCT_TYPE_ATLANTIS_D     PRODUCT_TYPE_ATLANTIS_D    /* DSR1020, DSR2020, DSR4020          */
#define OPTREC_PRODUCT_TYPE_IPMI_APP       PRODUCT_TYPE_IPMI_APPLIANCE/*                                    */
#define OPTREC_PRODUCT_TYPE_ATL_D_PHASE_B  PRODUCT_TYPE_ATL_D_PHASE_B /* DSR1020, DSR2020, DSR4020, DSR8020 */
#define OPTREC_PRODUCT_TYPE_ATL_D_8020     PRODUCT_TYPE_ATL_D_8020    /* DSR8020                            */
#define OPTREC_PRODUCT_TYPE_ATLANTIS_AHI   PRODUCT_TYPE_ATLANTIS_AHI  /* DSR1030, DSR2030, DSR4030, DSR8030 */
#define OPTREC_PRODUCT_TYPE_BV_AHI         PRODUCT_TYPE_BV_AHI        /* DSR1031, DSR1032                   */
#define OPTREC_PRODUCT_TYPE_TWIG_AHI       PRODUCT_TYPE_TWIG_AHI      /* DSR1034                            */
#define OPTREC_PRODUCT_TYPE_STINGRAY       PRODUCT_TYPE_STINGRAY      /* SVIP-1020                          */
#define OPTREC_PRODUCT_TYPE_NAUTILUS_D     PRODUCT_TYPE_NAUTILUS_D    /* Nautilus digital appliances        */
#define OPTREC_PRODUCT_TYPE_TRIDENT        PRODUCT_TYPE_TRIDENT       /* DSR2035, DSR8035                   */
#define OPTREC_PRODUCT_TYPE_AV_3x00        PRODUCT_TYPE_AV_3x00       /* AV3100, AV3200                   */
#define OPTREC_PRODUCT_TYPE_CYC_TSxK       PRODUCT_TYPE_CYC_TSxK      /* Cyclades TS400/800/1000/2000       */
#define OPTREC_PRODUCT_TYPE_CYC_TS1H       PRODUCT_TYPE_CYC_TS1H      /* Cyclades TS100/110                 */
#define OPTREC_PRODUCT_TYPE_CYC_TS3K       PRODUCT_TYPE_CYC_TS3K      /* Cyclades TS3000                    */
#define OPTREC_PRODUCT_TYPE_CYC_ACS        PRODUCT_TYPE_CYC_ACS       /* Cyclades ACS1/4/8/16/32/48         */
#define OPTREC_PRODUCT_TYPE_CYC_ONBOARD    PRODUCT_TYPE_CYC_ONBOARD   /* Cyclades OnBoard                   */
#define OPTREC_PRODUCT_TYPE_CYC_PM         PRODUCT_TYPE_CYC_PM        /* Cyclades PM                        */
#define OPTREC_PRODUCT_TYPE_CYC_KVMNET     PRODUCT_TYPE_CYC_KVMNET    /* Cyclades KVM/net 16/32             */
#define OPTREC_PRODUCT_TYPE_CYC_KVMPLUS    PRODUCT_TYPE_CYC_KVMPLUS   /* Cyclades KVM/net plus 16/32        */
#define OPTREC_PRODUCT_TYPE_DSI5200        PRODUCT_TYPE_DSI5200       /* Caesar Management Appliance        */
#define OPTREC_PRODUCT_TYPE_CYC_ONSITE     PRODUCT_TYPE_CYC_ONSITE    /* Cyclades OnSite                    */
#define OPTREC_PRODUCT_TYPE_ACS6000        PRODUCT_TYPE_ACS6000       /* ACS6000 (Golden Gate)              */
#define OPTREC_PRODUCT_TYPE_AMAZON         PRODUCT_TYPE_AMAZON        /* AMAZON (GSP)                       */
#define OPTREC_PRODUCT_TYPE_PMHD           PRODUCT_TYPE_PMHD          /* PMHD (Nazca/Stonehenge)            */
#define OPTREC_PRODUCT_TYPE_ACS5000        PRODUCT_TYPE_ACS5000       /* ACS5000                            */


#endif /* __FLASHHDR_H */

/* end */

