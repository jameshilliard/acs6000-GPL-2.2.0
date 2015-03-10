DESCRIPTION = "libnfsidmap package"
HOMEPAGE = "http://www.t2-project.org/packages/libnfsidmap.html"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.citi.umich.edu/projects/nfsv4/linux/libnfsidmap/libnfsidmap-0.20.tar.gz;localpath=${DL_DIR}/libnfsidmap-${PV}.tar.gz"

#SRC_URI += "file://mk.patch;patch=1"

S=${WORKDIR}/libnfsidmap-${PV}

inherit autotools

EXTRA_OECONF += "cross_compiling=yes DESTDIR=${D}"

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
	install -m 0644 ${S}/nfsidmap.h ${STAGING_INCDIR}
	install -m 0644 ${S}/.libs/libnfsidmap.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/.libs/libnfsidmap.a ${STAGING_LIBDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
