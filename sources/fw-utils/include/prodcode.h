//---------------------------------------------------------------------
// Copyright 2003, Avocent Corporation
//---------------------------------------------------------------------
// FILE         : prodcode.h
//
// DESCRIPTION  : Product Type codes
//
//---------------------------------------------------------------------
// AUTHOR       : David Stafford
// DATE         : Mar 6, 2003
//
//---------------------------------------------------------------------
// HISTORY
//
// DATE         PROGRAMMER        MODIFICATIONS
// Mar 6, 2003  David Stafford    Initial version
//
//---------------------------------------------------------------------

#ifndef __PRODCODE_H
#define __PRODCODE_H


/*
 * When adding Avocent-branded DSR PRODUCT_TYPE codes, update tersetest.c so
 *      the expected oemCode is reported to the DSRTestApp
 */

#define PRODUCT_TYPE_DS1800             1   /* DS1800                             */
#define PRODUCT_TYPE_DSR                2   /* DSR1161, DSR2161, and DSR4160      */
#define PRODUCT_TYPE_AVR                3   /* A1000R, A2000R, and OEM variations */
#define PRODUCT_TYPE_DSR800             4   /* DSR800                             */
#define PRODUCT_TYPE_DSR3               5   /* DSR1010, DSR2010, DSR4010          */
#define PRODUCT_TYPE_BVIEW              6   /* BranchView - DSR1021, DSR1022      */
#define PRODUCT_TYPE_CPSX10             7   /* CPSx10  CPS 810, CPS 1610          */
#define PRODUCT_TYPE_CPSX20             8   /* CPSx50  CPS 820, CPS 1620          */
#define PRODUCT_TYPE_TWIG               9   /* DSR1024                            */
#define PRODUCT_TYPE_ATLANTIS_D         10  /* DSR1020, DSR2020, DSR4020          */
#define PRODUCT_TYPE_ATLANTIS_A         11  /* Atlantis Analog                    */
#define PRODUCT_TYPE_IPMI_APPLIANCE     12  /* DSI5100                            */
#define PRODUCT_TYPE_ATL_D_PHASE_B      13  /* DSR1020, DSR2020, DSR4020, DSR8020 */
#define PRODUCT_TYPE_ATL_D_8020         14  /* DSR8020 */
#define PRODUCT_TYPE_CCM4850_GEODE_EQX  15  /* CCM4850 GEODE (EQX MIB)            */
#define PRODUCT_TYPE_CCMx50_PPC_EQX     16  /* CCM850, CCM1650 PPC (EQX MIB)      */
#define PRODUCT_TYPE_CCM4850_PPC_EQX    17  /* CCM4850 PPC (EQX MIB)              */
#define PRODUCT_TYPE_ATLANTIS_AHI       18  /* DSR1030, DSR2030, DSR4030, DSR8030 */
#define PRODUCT_TYPE_CCM4850_GEODE_AVCT 19  /* CCM4850 GEODE (AVCT MIB)           */
#define PRODUCT_TYPE_CCMx50_PPC_AVCT    20  /* CCM850, CCM1650 PPC (AVCT MIB)     */
#define PRODUCT_TYPE_CCM4850_PPC_AVCT   21  /* CCM4850 PPC (AVCT MIB)             */
#define PRODUCT_TYPE_BV_AHI             22  /* DSR1031, DSR1032                   */
#define PRODUCT_TYPE_TWIG_AHI           23  /* DSR1034                            */
#define PRODUCT_TYPE_STINGRAY           24  /* SVIP-1020                          */
#define PRODUCT_TYPE_NAUTILUS_D         25  /* Nautilus Digital appliances        */
#define PRODUCT_TYPE_TRIDENT            26  /* Trident DSR2035, DSR8035           */
#define PRODUCT_TYPE_AV_3x00            27  /* Amberbock AV3100, AV3200           */
#define PRODUCT_TYPE_CYC_TSxK		28  /* Cyclades TS400/800/1000/2000       */
#define PRODUCT_TYPE_CYC_TS1H		29  /* Cyclades TS100/110                 */
#define PRODUCT_TYPE_CYC_TS3K		30  /* Cyclades TS3000                    */
#define PRODUCT_TYPE_CYC_ACS		31  /* Cyclades ACS1/4/8/16/32/48         */
#define PRODUCT_TYPE_CYC_ONBOARD	32  /* Cyclades OnBoard                   */
#define PRODUCT_TYPE_CYC_PM			33  /* Cyclades PM                        */
#define PRODUCT_TYPE_CYC_KVMNET		34  /* Cyclades KVM/net 16/32             */
#define PRODUCT_TYPE_CYC_KVMPLUS	35  /* Cyclades KVM/net plus 16/32        */
#define PRODUCT_TYPE_DSI5200		36  /* Caesar Management Appliance        */
#define PRODUCT_TYPE_CYC_ONSITE		37  /* Cyclades OnSite                    */
#define PRODUCT_TYPE_ACS6000		38  /* ACS6000 (Golden Gate)              */
#define PRODUCT_TYPE_AMAZON			39  /* AMAZON (GSP)                       */
#define PRODUCT_TYPE_MP5300			40  /* MergePoint 5300                    */
#define PRODUCT_TYPE_MP_5324_5340	41  /* MergePoint 5324/5340               */
#define PRODUCT_TYPE_AV3050			42  /* Kitchen Sink AutoView 3050         */

#ifndef PRODUCT_TYPE_PMHD	//Defined in fw-utils[-native].bb to build for ACS
#define PRODUCT_TYPE_PMHD			43  /* PMHD (Nazca/Stonehenge)            */
#endif

#define PRODUCT_TYPE_ACS5000		44  /* ACS5000                            */
#define PRODUCT_TYPE_POLARIS		45  /* Polaris project family             */

#endif  /* __PRODCODE_H */

/* end */

