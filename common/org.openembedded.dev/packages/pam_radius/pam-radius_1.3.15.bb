DESCRIPTION = "pam_radius package"
HOMEPAGE = "http://www.freeradius.org"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "ftp://ftp.freeradius.org/pub/radius/old/pam_radius-${PV}.tar;localpath=${DL_DIR}/pam_radius-${PV}.tar"

SRC_URI += " file://mk.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1"
SRC_URI += " file://ipv6.patch;patch=1"
SRC_URI += " file://authorization_srvtype.patch;patch=1"
SRC_URI += " file://no_accounting.patch;patch=1"

S=${WORKDIR}/pam_radius-${PV}

DEPENDS = "libpam gsp-auth"

inherit autotools

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN} ${PN}-dev "

PARALLEL_MAKE = ""

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/etc/* \
	"

CFLAGS_prepend_powerpc = "-DHIGHFIRST "

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0755 ${S}/pam_*.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/radius.h ${STAGING_INCDIR}
}

do_install() {
	install -d ${D}/etc/raddb
	install -d ${D}/lib/security
	install -m 0755 ${S}/pam_*.so ${D}/lib/security
	install -m 0644 ${S}/pam*.conf ${D}/etc/raddb/server
}
