SECTION = "console/network"
DESCRIPTION = "netkit-ftp-usagi includes a commandline ftp client with IPV6."
LICENSE = "BSD"

SRC_URI = "ftp://ftp.uk.linux.org/pub/linux/Networking/netkit/netkit-ftp-usagi-${PV}.tar.gz \
	   file://mconfig.patch;patch=1 \
	   file://configure.patch;patch=1"

inherit autotools

EXTRA_OECONF = "--enable-ipv6"

EXTRA_OEMAKE = ""

export bindir=/bin

do_compile () {
	oe_runmake 'CC=${CC}' 'LD=${LD}' all
}

do_install () {
	install -d ${D}${bindir}
	install -m 0755 ftp/ftp ${D}${bindir}
}
