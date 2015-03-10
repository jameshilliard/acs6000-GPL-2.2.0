DESCRIPTION = "shadow"
SECTION = "sys-apps"
LICENSE = "GPL"
VIRTUAL_NAME ?= "shadow"
DEPENDS = "libpam"

SRC_URI = "ftp://ftp.pld.org.pl/software/shadow/old/shadow-${PV}.tar.bz2" 

COMPATIBLE_HOST = "i.86.*-linux"

inherit autotools gettext
PROVIDES += "${VIRTUAL_NAME}"

EXTRA_OECONF = "--without-selinux"
