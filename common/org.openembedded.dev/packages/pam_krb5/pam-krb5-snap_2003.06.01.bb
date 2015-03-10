DESCRIPTION = "pam_krb5 package"
HOMEPAGE = "http://sourceforge.net/projects/pam-krb5/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libpam krb5"
RDEPENDS = "libpam krb5"

SRC_URI = "${SOURCEFORGE_MIRROR}/pam_krb5/pam_krb5_snap-${PV}.tar.gz;localpath=${DL_DIR}/pam_krb5_snap-${PV}.tar.gz"

S=${WORKDIR}/pam_krb5_snap-${PV}

inherit autotools

#EXTRA_OECONF += "ac_cv_build_pam_krb5afs=yes BUILD_PAM_KRB5AFS=yes"

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/lib/security/pam_* \
	/etc/* \
	"
do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0755 ${S}/.libs/pam_*.so ${STAGING_LIBDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
