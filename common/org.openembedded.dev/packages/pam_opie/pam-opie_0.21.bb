DESCRIPTION = "pam_opie package"
HOMEPAGE = "http://www.inner.net/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.inner.net/pub/opie/contrib/pam_opie-${PV}.tar.gz"

SRC_URI += " file://mk.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1"

S=${WORKDIR}/pam_opie

DEPENDS = "opie libpam"
RDEPENDS = "opie libpam"

inherit autotools

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN} ${PN}-dev "

PARALLEL_MAKE = ""

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/etc/* \
	"

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0755 ${S}/pam_*.so ${STAGING_LIBDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D} FAKEROOT=${D}
}
