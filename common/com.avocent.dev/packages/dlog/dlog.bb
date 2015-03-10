# Module Description
DESCRIPTION = "Debug Log API for GSP"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "dlog"
SUB_MODULE_DIR = ""

# Package Configuration
PN = "dlog"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = ""
RDEPENDS = "syslog-ng"

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = "dlog.h"
EXPORT_LIBSO = "libdlog.so"
EXPORT_LIBA = "libdlog.a"

# Files to be packaged
INSTALL_FILES = "libdlog.so:${libdir}:755 dlogcfg:${bindir}:755"
FILES_${PN} = "${libdir}/libdlog.so ${bindir}/dlogcfg"

inherit avocentgsp
