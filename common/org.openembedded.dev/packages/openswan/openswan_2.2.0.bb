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
	   file://openswan-2.2.0-gentoo.patch;patch=1 \
           file://gcc4-fixes.patch;patch=1 \
	   file://makefile-whitespace-fix.patch;patch=1 \
	   file://ld-library-path-breakage.patch;patch=1"
S = "${WORKDIR}/openswan-${PV}"

PARALLEL_MAKE = ""
EXTRA_OEMAKE = "DESTDIR=${D} \
                USERCOMPILE="${CFLAGS}" \
                FINALCONFDIR=${sysconfdir}/ipsec \
                INC_RCDEFAULT=${sysconfdir}/init.d \
                INC_USRLOCAL=${prefix} \
                INC_MANDIR=share/man WERROR=''"

do_compile () {
	oe_runmake programs
}

do_install () {
	oe_runmake install
}

FILES_${PN} = "${sysconfdir} ${libdir}/ipsec/* ${sbindir}/* ${libexecdir}/ipsec/*"
FILES_${PN}-dbg += "${libdir}/ipsec/.debug ${libexecdir}/ipsec/.debug"

CONFFILES_${PN} = "${sysconfdir}/ipsec/ipsec.conf"
