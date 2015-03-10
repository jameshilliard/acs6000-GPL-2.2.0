# Module Description
DESCRIPTION = "Avocent Protocol Core Library - Provides Standardized Packet Parsing and Formation for the common Avocent protocol encapsulation format"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "avct-protocol-core"
SUB_MODULE_DIR = ""

# Package Configuration
PN = "avct-protocol-core"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = ""
RDEPENDS = ""

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = "avct-protocol-core.h"
EXPORT_LIBSO = ""
EXPORT_LIBA = "libavct-protocol-core.a"

# Files to be packaged
INSTALL_FILES = ""
FILES_${PN} = ""

inherit avocentgsp
