DESCRIPTION = "rpcbind package"
HOMEPAGE = ""
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libtirpc"

SRC_URI = "http://nfsv4.bullopensource.org/tarballs/rpcbind/rpcbind-0.1.4.tar.bz2;localpath=${DL_DIR}/rpcbind-${PV}.tar.bz2"

SRC_URI += " file://tirpc.patch;patch=1"

S=${WORKDIR}/rpcbind-${PV}

inherit autotools

EXTRA_OECONF += "cross_compiling=yes"

PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/usr/* \
	"

do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake TIRPCDIR=${STAGING_INCDIR}/tirpc
}

do_install() {
	oe_runmake install DESTDIR=${D}
}

