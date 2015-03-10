# Module Description
DESCRIPTION = "ADSAP2 Library"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "avct-adsap2-lib"
SUB_MODULE_DIR = ""

# Package Configuration
PN = "avct-adsap2-lib"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = "gsp-air"
RDEPENDS = ""

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = "adsap2.h adsap2_fn.h adsap2.xml"
EXPORT_LIBSO = ""
EXPORT_LIBA = "libadsap2.a"

# Files to be packaged
INSTALL_FILES = ""
FILES_${PN} = ""

CFLAGS += " -I."
inherit avocentgsp
