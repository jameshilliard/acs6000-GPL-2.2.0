DESCRIPTION = "Samba package"
HOMEPAGE = "http://www.samba.org/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://us1.samba.org/samba/ftp/samba-${PV}.tar.gz;localpath=${DL_DIR}/samba-${PV}.tar.gz"

SRC_URI += " file://mk.patch;patch=1"
SRC_URI += " file://configure.patch;patch=1"

S=${WORKDIR}/samba-${PV}/source

libdir=/lib

inherit autotools

EXTRA_OECONF += "cross_compiling=yes"

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/lib/*.so* \
	/usr/lib/*.so* \
	/etc/* \
	/bin/* \
	/usr/bin/* \
	"

oe_runmake() {
    if [ x"$MAKE" = x ]; then MAKE=make; fi
    oenote make "$@"
    make "$@" || die "oe_runmake failed"
}

do_configure() {
	oe_runconf
}

do_stage() {
	install -m 0755 ${S}/bin/libsmbclient.so ${STAGING_LIBDIR}
	install -d 0755 ${STAGING_INCDIR}/smb
	install -m 0755 ${S}/include/* ${STAGING_INCDIR}/smb
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
