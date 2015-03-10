DESCRIPTION = "A library implementing GSP AAA functionality."

EXPORT_H  = "gsp_auth.h gsp_acl.h gsp_aaa.h auth_priv.h"

EXPORT_LIBSO = "libgsp_auth.so libgsp_auth.so.0"

EXPORT_LIBA = "libgsp_auth.a"

INSTALL_FILES = "libgsp_auth.so:/usr/lib:0755"

MODULE_NAME = gsp-auth

DEPENDS = "libpam libcyc libdaemon avct-adsap2-lib dlog"
RDEPENDS = "gsp-air"

inherit avocentgsp

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};rev=209;localpath=${DL_DIR}/${MODULE_NAME};proto=https"

S=${WORKDIR}/${MODULE_NAME}

do_install_append() {
	ln -sf libgsp_auth.so ${D}/usr/lib/libgsp_auth.so.0
}
PACKAGES = "${PN}"
FILES_${PN} = "${libdir}/*.so*"

DEBUG_CFLAGS="-DAUTH_DEBUG=0 "
#DEBUG_CFLAGS += "-g -ggdb "

CFLAGS_prepend =" -Wall -Werror -I. -D_REENTRANT -fPIC ${DEBUG_CFLAGS}"
CXXFLAGS_prepend =" -Wall -Werror -I. -D_REENTRANT -fPIC ${DEBUG_CFLAGS}"
