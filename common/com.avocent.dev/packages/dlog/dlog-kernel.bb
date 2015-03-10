# Module Description
DESCRIPTION = "Kernel Debug Log API for GSP"
SECTION = "generic"
LICENSE = "Avocent Proprietary"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "eduardo.fernandez@avocent.com"

# Module Info
MODULE_NAME = "dlog-kernel"
SUB_MODULE_DIR = ""

# Package Configuration
PN = "dlog-kernel"
PV = "1.0"
PR = "r1"

# Dependencies
DEPENDS = ""
RDEPENDS = ""

# Source Locations
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=dlog;localpath=${DL_DIR}/dlog"
S=${WORKDIR}/dlog/${SUB_MODULE_DIR}

inherit module

# This package merely "installs" the Dlog header file into the kernel staging include directory. This way modules
# may access the Dlog Kernel API
do_install() {
}

do_stage_append() {
        install -m 0644 dlog.h ${KERNEL_SOURCE}/include
}


