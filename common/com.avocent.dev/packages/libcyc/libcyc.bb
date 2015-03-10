DESCRIPTION = "A general-purpose utility library that may be used with Avocent applications."

EXPORT_H = "../include/cyc.h ../include/avo_defs.h ../include/cyc++.h ../include/md5.h"

#The following line means copy libsample.so to ${STAGING_LIBDIR}
EXPORT_LIBSO = "libcyc.so.3.0 libcyc++.so.0.1"

#The following line means  copy libsample.a to ${STAGING_LIBDIR}
EXPORT_LIBA = "libcyc.a libcyc++.a"

#The following line means install sample.exe to /usr/bin
INSTALL_FILES = "libcyc.so.*:/usr/lib:0755 libcyc++.so.*:/usr/lib:0755"

# The module's name
MODULE_NAME = libcyc
SUB_MODULE_DIR = src

# 
DEPENDS = ""
#
RDEPENDS = ""

inherit avocentgsp

do_install_append() {
	ln -sf libcyc.so.3.0 ${D}/usr/lib/libcyc.so.3
	ln -sf libcyc++.so.0.1 ${D}/usr/lib/libcyc++.so.0
}

#Change the 'tmpl' in following line, it's the dir name of your module under ~/sources
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=libcyc;rev=209;localpath=${DL_DIR}/${MODULE_NAME};proto=https"

#Change the 'tmpl' in the following line, typically, the makefile locates in the directory ${S}, you may
#add subdir under 'tmpl' according to your module.
S=${WORKDIR}/${MODULE_NAME}/${SUB_MODULE_DIR}

PACKAGES = "${PN} "
FILES_${PN} = "${libdir}/*.so*"

do_compile () {
        make -C ${S} all
}

