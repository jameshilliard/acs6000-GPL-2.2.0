DESCRIPTION = "am_unix package"
HOMEPAGE = "http://www.thkukuk.de/pam/pam_unix2/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

DEPENDS = "libpam libxcrypt"
RDEPENDS = "libpam libxcrypt"

SRC_URI = "ftp://ftp.suse.com/pub/people/kukuk/pam/pam_unix2/pam_unix2-${PV}.tar.bz2;localpath=${DL_DIR}/pam_unix2-${PV}.tar.bz2"

#SRC_URI += " file://nostrip_fix.patch;patch=1"
SRC_URI += " file://gsp_aaa.patch;patch=1 \
	     file://ja_po.patch;patch=1 \
	   "

S=${WORKDIR}/pam_unix2-${PV}

libdir=/lib

inherit autotools

EXTRA_OECONF += "cross_compiling=yes"

# This is crude - the modules maybe should each have independent ipks
PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/lib/security/pam_*.so \
	/usr/share/locale/ja_JP* \
	/usr/share/locale/zh_CN* \
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

do_install_acs6000() {
	oe_runmake install DESTDIR=${D}
	install -d ${D}/usr/share
	install -d ${D}/usr/share/locale
	install -d ${D}/usr/share/locale/ja_JP.UTF-8
	install -d ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES
	install -m 0644 ${S}/po/ja.gmo ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES/pam_unix2.mo
	install -d ${D}/usr/share/locale/zh_CN.UTF-8
	install -d ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES
	install -m 0644 ${S}/po/zh_CN.gmo ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES/pam_unix2.mo
}
