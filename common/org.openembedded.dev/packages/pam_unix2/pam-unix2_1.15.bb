DESCRIPTION = "am_unix package"
HOMEPAGE = "http://www.thkukuk.de/pam/pam_unix2/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libpam libxcrypt"
RDEPENDS = "libpam libxcrypt"

SRC_URI = "ftp://ftp.suse.com/pub/people/kukuk/pam/pam_unix2/pam_unix2-${PV}.tar.bz2;localpath=${DL_DIR}/pam_unix2-${PV}.tar.gz"

SRC_URI += " file://nostrip_fix.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1"
SRC_URI += " file://multibyte_unix_auth.patch;patch=1"

S=${WORKDIR}/pam_unix2-${PV}

libdir=/lib

inherit autotools

EXTRA_OECONF += "cross_compiling=yes"

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/etc/* \
	"

do_compile() {
	oe_runmake
}

do_stage() {
	install -m 0755 ${S}/src/pam_unix2.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/src/*.h ${STAGING_INCDIR}
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
