DESCRIPTION = "Password Checking Library"
AUTHOR = "Nathan Neulinger <nneul@umr.edu> Alec Muffett <alecm@crypto.dircon.co.uk>"
HOMEPAGE = "http://sourceforge.net/projects/cracklib"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
SECTION = "lib"
PRIORITY = "optional"
LICENSE = "GPLv2"
DEPENDS = "cracklib-native"
PR = "r1"

SRC_URI = "${SOURCEFORGE_MIRROR}/cracklib/cracklib-${PV}.tar.gz \
           file://busybox-gzip.patch;patch=1 \
          "

S = ${WORKDIR}/cracklib-${PV}

EXTRA_OECONF += " --enable-nls "

inherit autotools

FILES_${PN} += "\
        /usr/share/locale/ja_JP* \
        /usr/share/locale/zh_CN* \
        "

do_install_append () {
    install -d ${D}${datadir}/${PN}
    install -m 0644 dicts/cracklib-small ${D}${datadir}/${PN}

	cracklib-format dicts/cracklib-small | cracklib-packer ${D}${libdir}/cracklib_dict

	install -d ${D}/usr/share
	install -d ${D}/usr/share/locale
	install -d ${D}/usr/share/locale/ja_JP.UTF-8
	install -d ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES
	install -m 0644 ${S}/po/ja.gmo ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES/cracklib.mo
	install -d ${D}/usr/share/locale/zh_CN.UTF-8
	install -d ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES
	install -m 0644 ${S}/po/zh_CN.gmo ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES/cracklib.mo
}

do_stage () {
	install -m 0644 lib/crack.h ${STAGING_INCDIR}
	oe_libinstall libcrack ${STAGING_LIBDIR}
}

#FILES_${PN} += "${libdir}/cracklib_dict.* ${datadir}/${PN}/cracklib-small"
FILES_${PN} += "${datadir}/${PN}/cracklib-small"

pkg_postinst_${PN} () {
if test "x$D" != "x"; then
	exit 1
fi
}

