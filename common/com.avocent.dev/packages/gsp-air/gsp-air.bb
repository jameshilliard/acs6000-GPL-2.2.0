# Module Description
DESCRIPTION = "GSP Avocent Intrasystem RPC Library"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "gsp-air"
SUB_MODULE_DIR = "src"

# Package Configuration
PN = "gsp-air"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = "gsp-cair dlog"
RDEPENDS = "dlog"

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = "gsp-air.h"
EXPORT_LIBSO = "libgsp-air.so"
EXPORT_LIBA =

# Files to be packaged
INSTALL_FILES = "libgsp-air.so:${libdir}:755"
FILES_${PN} = "${libdir}/libgsp-air.so"

inherit avocentgsp
