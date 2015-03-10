DESCRIPTION = "pam_ldap package"
HOMEPAGE = "http://www.padl.com/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libpam openldap gsp-auth"

SRC_URI = "http://www.padl.com/download/pam_ldap-${PV}.tar.gz;localpath=${DL_DIR}/pam_ldap-${PV}.tar.gz"

SRC_URI += " file://mk.patch;patch=1"
SRC_URI += " file://ldap.conf.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1"

S=${WORKDIR}/pam_ldap-${PV}

libdir=/lib

inherit autotools

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN} ${PN}-dev "

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/etc/* \
	"
LDFLAGS_append = " -lssl -lcrypto"

export LDADD = "-lgsp_auth -lpam_misc"

EXTRA_OECONF = "--enable-ssl"

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0755 ${S}/pam_ldap.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/pam_ldap.h ${STAGING_INCDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
