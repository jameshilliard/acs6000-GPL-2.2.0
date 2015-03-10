static struct OemTable
{
  char *name;
  int code;
}
oemTable[] =
{
  { "Avocent",            OPTREC_OEM_AVOCENT },
  { "Avo",                OPTREC_OEM_AVOCENT },
  { "Compaq",             OPTREC_OEM_COMPAQ },
  { "Dell",               OPTREC_OEM_DELL },
  { "HP",                 OPTREC_OEM_HP },
  { "IBM",                OPTREC_OEM_IBM },
  { "Siemens",            OPTREC_OEM_SMS },
  { "FSC",                OPTREC_OEM_SMS },
  { "WL",                 OPTREC_OEM_WL },
  { "BLACKBOX",           OPTREC_OEM_BLACKBOX },
  { "BBOX",               OPTREC_OEM_BLACKBOX },
  { "FCL",		  OPTREC_OEM_FCL },
  { "TEST",               OPTREC_OEM_TESTRACK },
};


static struct ProductTable
{
  char *name;
  int code;
}
prodTable[] =
{
  { "DS1800",             OPTREC_PRODUCT_TYPE_DS1800 },
  { "DSR1",               OPTREC_PRODUCT_TYPE_DSR },
  { "DSR",                OPTREC_PRODUCT_TYPE_DSR },
  { "DSR2",               OPTREC_PRODUCT_TYPE_AVR },
  { "AVR",                OPTREC_PRODUCT_TYPE_AVR },
  { "DSR800",             OPTREC_PRODUCT_TYPE_DSR800 },
  { "DSR3",               OPTREC_PRODUCT_TYPE_DSR3 },
  { "BV",                 OPTREC_PRODUCT_TYPE_BVIEW },
  { "TWIG",               OPTREC_PRODUCT_TYPE_TWIG },
  { "ATLANTIS_D",         OPTREC_PRODUCT_TYPE_ATLANTIS_D },
  { "ATLANTIS_A",         OPTREC_PRODUCT_TYPE_ATLANTIS_A },
  { "IPMI_APP",           OPTREC_PRODUCT_TYPE_IPMI_APP },
  { "ATL_D_PHASE_B",      OPTREC_PRODUCT_TYPE_ATL_D_PHASE_B },
  { "ATL_D_8020",         OPTREC_PRODUCT_TYPE_ATL_D_8020 },
  { "ATLANTIS_AHI",       OPTREC_PRODUCT_TYPE_ATLANTIS_AHI },
  { "BV_AHI",             OPTREC_PRODUCT_TYPE_BV_AHI },
  { "TWIG_AHI",           OPTREC_PRODUCT_TYPE_TWIG_AHI },
  { "STINGRAY",           OPTREC_PRODUCT_TYPE_STINGRAY },
  { "NAUTILUS_D",         OPTREC_PRODUCT_TYPE_NAUTILUS_D },
  { "TRIDENT",            OPTREC_PRODUCT_TYPE_TRIDENT },
  { "AV3x00",             OPTREC_PRODUCT_TYPE_AV_3x00 },
  { "CYC_TSxK",           OPTREC_PRODUCT_TYPE_CYC_TSxK },
  { "CYC_TS1H",           OPTREC_PRODUCT_TYPE_CYC_TS1H },
  { "CYC_TS3K",           OPTREC_PRODUCT_TYPE_CYC_TS3K },
  { "CYC_ACS",            OPTREC_PRODUCT_TYPE_CYC_ACS },
  { "CYC_ONBOARD",        OPTREC_PRODUCT_TYPE_CYC_ONBOARD },
  { "CYC_PM",             OPTREC_PRODUCT_TYPE_CYC_PM },
  { "CYC_KVMNET",         OPTREC_PRODUCT_TYPE_CYC_KVMNET },
  { "CYC_KVMPLUS",        OPTREC_PRODUCT_TYPE_CYC_KVMPLUS },
  { "DSI5200",            OPTREC_PRODUCT_TYPE_DSI5200 },
  { "CYC_ONSITE",         OPTREC_PRODUCT_TYPE_CYC_ONSITE },
  { "ACS6000",            OPTREC_PRODUCT_TYPE_ACS6000 },
  { "AMAZON",             OPTREC_PRODUCT_TYPE_AMAZON },
  { "PMHD",               OPTREC_PRODUCT_TYPE_PMHD },
  { "ACS5K",              OPTREC_PRODUCT_TYPE_ACS5000 },

};

