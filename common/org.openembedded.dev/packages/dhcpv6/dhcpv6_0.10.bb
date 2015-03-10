DESCRIPTION   =  "Server and client for DHCPv6"
HOMEPAGE      =  "http://sourceforge.net/projects/dhcpv6/"
SECTION       =  "console/network"
PRIORITY      =  "optional"
LICENSE       =  "GPL-2"
MAINTAINER    =  "Lynn Lin <lynn.lin@avocent.com>"
PR            =  "r0"

SRC_URI    =	"http://nchc.dl.sourceforge.net/sourceforge/dhcpv6/dhcp-${PV}.tgz \
	        " 
S = ${WORKDIR}/dhcp-${PV}

FILES_${PN} += "/usr/local/sbin/dhcp6*"

do_configure () {
	./configure --host=${TARGET_SYS}
}
do_compile () {
        unset CFLAGS
        oe_runmake bindir=${bindir} sbindir=${sbindir} mandir=${mandir}
}

do_install () {
	install -d ${D}/usr/local/sbin ${D}${sysconfdir}/init.d
	install dhcp6c dhcp6s ${D}/usr/local/sbin
	install dhcp6c.sh ${D}${sysconfdir}/init.d/dhcp6c
	install dhcp6s.sh ${D}${sysconfdir}/init.d/dhcp6s
	install dhcp6c.conf dhcp6s.conf ${D}${sysconfdir}
}

