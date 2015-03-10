SECTION = "console/network"
DESCRIPTION = "linux-ftpd-usagi includes a commandline ftpd server with IPV6."
LICENSE = "BSD"

SRC_URI = "ftp://ftp.uk.linux.org/pub/linux/Networking/netkit/linux-ftpd-usagi-${PV}.tar.gz \
	   file://configure.patch;patch=1 \
	"

inherit autotools

EXTRA_OECONF = "--enable-ipv6"

EXTRA_OEMAKE = ""

export bindir=/sbin

do_compile () {
	oe_runmake 'CC=${CC}' 'LD=${LD}' all
}

do_install () {
	install -d ${D}${bindir}
	install -m 0755 ftpd/ftpd ${D}${bindir}
}
