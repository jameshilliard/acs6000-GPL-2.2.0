DESCRIPTION = "libevent package"
HOMEPAGE = "https://sourceforge.net/projects/levent"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://jaist.dl.sourceforge.net/sourceforge/levent/libevent-${PV}.tar.gz;localpath=${DL_DIR}/libevent-${PV}.tar.gz"

SRC_URI += "file://mk.patch;patch=1"

S=${WORKDIR}/libevent-${PV}

inherit autotools

EXTRA_OECONF += "cross_compiling=yes --prefix=${D}"

PACKAGES = "${PN}"

FILES_${PN} = "\
	/usr/lib/* \
	"

do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0644 ${S}/event.h ${STAGING_INCDIR}
	install -m 0644 ${S}/libevent.a ${STAGING_LIBDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
