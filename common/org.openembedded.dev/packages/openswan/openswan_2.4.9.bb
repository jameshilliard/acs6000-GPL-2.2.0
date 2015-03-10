SECTION = "console/network"
DESCRIPTION = "Openswan is an Open Source implementation of IPsec for the \
Linux operating system."
HOMEPAGE = "http://www.openswan.org"
LICENSE = "GPLv2"
MAINTAINER = "Bruno Randolf <bruno.randolf@4g-systems.biz>"
DEPENDS = "gmp flex-native"
RRECOMMENDS = "kernel-module-ipsec"
RDEPENDS_nylon = "perl"
PR = "r5"

SRC_URI = "http://www.openswan.org/download/old/openswan-${PV}.tar.gz \
           file://ipsec.conf \
	"
S = "${WORKDIR}/openswan-${PV}"

PARALLEL_MAKE = ""
EXTRA_OEMAKE = "DESTDIR=${D} \
                USERCOMPILE="${CFLAGS}" \
                FINALCONFDIR=${sysconfdir} \
                INC_RCDEFAULT=${sysconfdir}/init.d \
                INC_USRLOCAL=${prefix} \
                INC_MANDIR=share/man WERROR=''"

do_compile () {
	oe_runmake programs
}

do_install () {
	oe_runmake install
	install -m 0644 ${WORKDIR}/ipsec.conf ${D}${sysconfdir}/ipsec.conf
}

do_install_append() {
	# remove links to init script (we do not want ipsec to start by default):
	rm -rf ${D}/etc/rc{S,0,1,2,3,4,5,6}.d
}

pkg_postinst () {
	if test "x$D" != "x"; then
		exit 1
	else
		echo " Generating IPSEC host key ..."
		/usr/sbin/ipsec newhostkey --output /etc/ipsec.secrets --random /dev/urandom
		echo "include /etc/ipsec.psk.secrets" >> /etc/ipsec.secrets
		touch /etc/ipsec.psk.secrets
	fi
}

FILES_${PN} = "${sysconfdir} ${libdir}/ipsec/* ${sbindir}/* ${libexecdir}/ipsec/*"
FILES_${PN}-dbg += "${libdir}/ipsec/.debug ${libexecdir}/ipsec/.debug"

CONFFILES_${PN} = "${sysconfdir}/ipsec.conf"
