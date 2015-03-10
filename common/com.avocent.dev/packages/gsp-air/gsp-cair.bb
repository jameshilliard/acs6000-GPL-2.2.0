# Module Description
DESCRIPTION = "GSP Avocent Intrasystem RPC Code Generation Tool"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "gsp-air"
SUB_MODULE_DIR = "tools"

# Package Configuration
PN = "gsp-cair"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = ""
RDEPENDS = ""

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

# Exported Items
EXPORT_H = 
EXPORT_LIBSO = 
EXPORT_LIBA =

# Files to be packaged
INSTALL_FILES = 
FILES_${PN} = 

inherit cross

# This package merely "installs" the Code Generation Script into the cross tools dir
do_install() {
}

do_stage() {
        install -d ${bindir}
        install -m 0755 gsp-cair.py ${bindir}
}
