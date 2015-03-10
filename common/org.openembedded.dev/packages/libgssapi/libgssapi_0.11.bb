DESCRIPTION = "libgssapi package"
HOMEPAGE = "http://www.citi.umich.edu/projects/nfsv4/linux/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.citi.umich.edu/projects/nfsv4/linux/libgssapi/libgssapi-${PV}.tar.gz;localpath=${DL_DIR}/libgssapi-${PV}.tar.gz"

#SRC_URI += "file://mk.patch;patch=1"

S=${WORKDIR}/libgssapi-${PV}

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
	install -d ${STAGING_LIBDIR}/pkgconfig ${STAGING_INCDIR}/gssapi
	install -m 0644 ${S}/src/gssglue/gssapi/gssapi.h ${STAGING_INCDIR}/gssapi
	install -m 0644 ${S}/src/.libs/libgssapi.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/src/.libs/libgssapi.a ${STAGING_LIBDIR}
	install -m 0644 ${S}/libgssapi.pc ${STAGING_LIBDIR}/pkgconfig
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
