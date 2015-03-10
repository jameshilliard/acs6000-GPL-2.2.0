DESCRIPTION = "WIDE-DHCPv6"
HOMEPAGE = "http://wide-dhcpv6.sourceforge.net/"
SECTION = "console/network"
LICENSE = "BSD"
PR = "r1"



SRC_URI = "http://superb-west.dl.sourceforge.net/sourceforge/wide-dhcpv6/wide-dhcpv6-${PV}.tar.gz \
           file://wide-dhcpv6-20070507.patch;patch=1 \
           file://moving-duid-file.patch;patch=1 \
           file://dhcp6c.conf \
           file://dhcp6c-opt.conf \
           file://dhcpv6-opt.conf \
           file://dhcpv6-opt.sh \
           file://dhcpv6-opt-set.sh"

PARALLEL_MAKE=""

do_configure() {
	./configure --host=${TARGET_SYS}
}

do_compile () {
	unset CFLAGS
	oe_runmake bindir=${bindir} sbindir=${sbindir} mandir=${mandir}
}

do_install () {
	unset CFLAGS
	oe_runmake bindir=${D}${bindir} sbindir=${D}${sbindir} mandir=${D}${mandir} install
	install -d ${D}${sysconfdir}
	install -d ${D}/var/db
	install -m 0644 ${WORKDIR}/dhcp6c.conf ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/dhcp6c-opt.conf ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/dhcpv6-opt.conf ${D}${sysconfdir}
	install -m 0750 ${WORKDIR}/dhcpv6-opt.sh ${D}${sbindir}
	install -m 0750 ${WORKDIR}/dhcpv6-opt-set.sh ${D}${sbindir}
}
