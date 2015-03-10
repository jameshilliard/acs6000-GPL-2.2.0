# Module Description
DESCRIPTION = "ADSAP2 External Library Sample"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "avct-adsap2-ext-sample"
SUB_MODULE_DIR = ""

# Package Configuration
PN = "avct-adsap2-ext-sample"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = "avct-adsap2"
RDEPENDS = "dlog avct-adsap2"

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"

S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = ""
EXPORT_LIBSO = ""
EXPORT_LIBA = ""

# Files to be packaged
INSTALL_FILES = "libadsap2ext.so:${libdir}:755"
FILES_${PN} = "${libdir}/libadsap2ext.so"

inherit avocentgsp
