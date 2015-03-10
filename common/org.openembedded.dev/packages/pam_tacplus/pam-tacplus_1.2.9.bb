DESCRIPTION = "pam_tacplus package"
HOMEPAGE = "http://echelon.pl"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libpam gsp-auth"

SRC_URI = "http://echelon.pl/pubs/pam_tacplus-${PV}.tar.gz;localpath=${DL_DIR}/pam_tacplus-${PV}.tar.gz"

SRC_URI += " file://mk.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1"
SRC_URI += " file://ipv6.patch;patch=1"
SRC_URI += " file://tacV0_and_authLevel.patch;patch=1"
SRC_URI += " file://tacacs.patch;patch=1"
SRC_URI += " file://no_accounting.patch;patch=1"
SRC_URI += " file://tacplus.conf"

S=${WORKDIR}/pam_tacplus

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
	install -m 0644 ${S}/pam_*.h ${STAGING_INCDIR}
}

do_install() {
	install -d ${D}/lib/security
	install -m 0755 ${S}/pam_*.so ${D}/lib/security/
	install -D -m 0644 ${WORKDIR}/tacplus.conf ${D}/etc/tacplus.conf
}
