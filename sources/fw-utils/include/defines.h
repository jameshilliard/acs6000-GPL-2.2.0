//---------------------------------------------------------------------
// Copyright 2002, Avocent Corporation
//---------------------------------------------------------------------
// FILE         : defines.h
//
// DESCRIPTION  : DS appliance conditional compile definitions
//
//---------------------------------------------------------------------
// AUTHOR       : David Stafford
// DATE         : Jan-04-2002
//
//---------------------------------------------------------------------
// HISTORY
//
// DATE         PROGRAMMER        MODIFICATIONS
// Jan-04-2002  David Stafford    Initial version
//
//---------------------------------------------------------------------

#ifndef __DEFINES_H
#define __DEFINES_H

/*
 * enforce View Client selection
 */
#undef __VIEW_CLIENT_DEFINED

#ifdef DS_VIEW_CLIENT
  #ifdef __VIEW_CLIENT_DEFINED
    #error Multiple clients defined
  #endif
  #define __VIEW_CLIENT_DEFINED
#endif

#ifdef AVWORKS_CLIENT
  #ifdef __VIEW_CLIENT_DEFINED
    #error Multiple clients defined
  #endif
  #define __VIEW_CLIENT_DEFINED
#endif

#ifndef __VIEW_CLIENT_DEFINED
  #error No View client defined
#endif


/*
 * enforce Platform selection
 */
#undef __PLATFORM_DEFINED

#ifdef DSR3_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef DSR800_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef DSR_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef AVR_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef DS1800_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef BVIEW_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef ATLANTIS_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef TRIDENT_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef TWIG_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef BVIEW_AHI_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifdef STINGRAY_PLATFORM
  #ifdef __PLATFORM_DEFINED
    #error Multiple hardware platforms defined
  #endif
  #define __PLATFORM_DEFINED
#endif

#ifndef __PLATFORM_DEFINED
  #error No Hardware platform defined
#endif

/*
 * detect platform type and set PRODUCT_TYPE_ID_CODE appropriately
 */
#ifdef DS1800_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_DS1800
#endif

#ifdef DSR_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_DSR
  #define RCM_SUBSYSTEM 1
  #define DSR_REV1 1          /* 1st edition of DSR hardware */
#endif

#ifdef AVR_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_AVR
  #define RCM_SUBSYSTEM 1
  #define DSR_REV2 1          /* 2nd edition of DSR hardware */
#endif

#ifdef DSR800_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_DSR800
  #define RCM_SUBSYSTEM 1
  #define DSR_REV2 1          /* 2nd edition of DSR hardware */
#endif

#ifdef DSR3_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_DSR3
  #define RCM_SUBSYSTEM 1
  #define DSR_REV3 1          /* 3rd edition of DSR hardware */
#endif

#ifdef BVIEW_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_BVIEW
  #define RCM_SUBSYSTEM 1
#endif

#ifdef ATLANTIS_PLATFORM
  #if defined (ATLANTIS_AHI_BOARD)
    #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_ATLANTIS_AHI     /* DSR1030, DSR2030, DSR4030, DSR8030 */
  #elif defined (ATL_8CH_208)
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_ATL_D_8020       /* DSR8020           */
  #elif defined (AUTOVIEW_3x00)
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_AV_3x00          /* AV3100, AV3200    */
  #else
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_ATLANTIS_D       /*  DSR1020, DSR2020, DSR4020 */
  #endif
  #define RCM_SUBSYSTEM 1
#endif

#ifdef TRIDENT_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_TRIDENT     /* DSR2035, DSR8035 */
  #define RCM_SUBSYSTEM 1
#endif

#ifdef TWIG_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_TWIG
  #define RCM_SUBSYSTEM 1
#endif

#ifdef STINGRAY_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_STINGRAY
  #define RCM_SUBSYSTEM 1
#endif


#ifdef BVIEW_AHI_PLATFORM
  #define PRODUCT_TYPE_ID_CODE  PRODUCT_TYPE_BV_AHI
  #define RCM_SUBSYSTEM 1
#endif

#ifndef PRODUCT_TYPE_ID_CODE
  #error PRODUCT_TYPE_ID_CODE is not defined
#endif


#endif /* __DEFINES_H */

/* end */

