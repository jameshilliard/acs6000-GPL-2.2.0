DESCRIPTION = "opie 2.4 package"
HOMEPAGE = "http://www.inner.net/opie"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = "NRL"
PR = "r1"

DEPENDS="gsp-auth"
#RDEPENDS="gsp-auth"

SRC_URI = "http://www.inner.net/pub/opie/opie-${PV}.tar.gz;localpath=${DL_DIR}/opie-${PV}.tar.gz"

python () {
    uri = bb.data.getVar("SRC_URI", d, 1)
    sharecode = bb.data.getVar('SHARECODE', d, 1)

    if sharecode != 'GSP':
        uri += " file://bison-mk-errno.patch;patch=1 file://keyfile.patch;patch=1 file://login_su_ftpd.patch;patch=1"
    if sharecode == 'GSP':
    	uri += " file://opie-2.4.diff;patch=1 file://retries-for-inv-user.diff;patch=1"
    if sharecode == 'GSP':
    	uri += " file://gsp-aaa.patch;patch=1"
    bb.data.setVar('SRC_URI', uri, d)
}

LDFLAGS_prepend = "-lpam -lpam_misc -lgsp_auth -lcyc -lcyc++ -lstdc++ -lcyevent -ldlog "

S=${WORKDIR}/opie-${PV}

inherit autotools

FILES_${PN} = "\
	/usr/* \
	"

FILES_${PN}_GSP = "\
	/usr/* \
	/bin/opielogin \
	/etc/* \
	"

do_configure() {
	oe_runconf cross_compiling=yes
}

do_configure_GSP() {
	oe_runconf "--enable-insecure-override"
}

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake client-install DESTDIR=${D} LOCALBIN=${D}/usr/bin LOCALMAN=${D}/usr/share/man OWNER=`id -nu` GROUP=`id -ng`
	oe_runmake server-install DESTDIR=${D} LOCALBIN=${D}/usr/bin LOCALMAN=${D}/usr/share/man LOCK_DIRS=${D}/etc/opielocks OWNER=`id -nu` GROUP=`id -ng` LOGIN=${D}/bin/login SU=${D}/bin/su FTPD=${D}/bin/ftpd
}

do_install_append() {
        ln -sf opiekey ${D}/usr/bin/otp-md4
	ln -sf opiekey ${D}/usr/bin/otp-md5
}

do_install_GSP() {
	install -d ${D}/bin
	install -m 4511 opielogin ${D}/bin/
	install -d ${D}/usr/local/bin
	install -m 4511 opiepasswd ${D}/usr/local/bin/opiepasswd
	install -m 0511 opiekey ${D}/usr/local/bin/opiekey
	install -d ${D}/etc
	install -d -m 0700 ${D}/etc/opielocks
	touch ${D}/etc/opiekeys
	chmod 0644 ${D}/etc/opiekeys
}

do_stage() {
	install -m 0644 ${S}/libopie/libopie.a ${STAGING_LIBDIR}
	install -m 0644 ${S}/opie.h ${STAGING_INCDIR}
}

pkg_postinst_opie_GSP() {
}

pkg_postinst_opie() {
if test "x$D" = "x"; then
	chmod 4111 /bin/su /bin/login /bin/ftpd
else
        exit 1
fi
}

