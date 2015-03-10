DESCRIPTION = "pam_smb package"
HOMEPAGE = "http://www.csn.ul.ie/~airlied/pam_smb/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "samba libpam"
RDEPENDS = "samba libpam"

SRC_URI = "http://www.csn.ul.ie/~airlied/pam_smb/pam_smb-${PV}.tar.gz;localpath=${DL_DIR}/pam_smb-${PV}.tar.gz"

SRC_URI += " file://gsp_aaa.patch;patch=1"

S=${WORKDIR}/pam_smb-${PV}

libdir=/lib

inherit autotools

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/etc/* \
	"

do_configure() {
	oe_runconf
}

do_stage() {
	install -m 0755 ${S}/pam_*.so ${STAGING_LIBDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
