SECTION = "console/network"
DESCRIPTION = "port of the OpenBSD TFTP server" 
LICENSE = "BSD"
SRC_URI = "http://www.kernel.org/pub/software/network/tftp/${P}.tar.bz2"

DEPENDS += "readline update-rc.d"
RDEPENDS += "update-rc.d"

COMPATIBLE_HOST = ".*-linux"

FILES_tftp-hpa = "/tftpboot/ ${bindir} ${sbindir}"

#inherit autotools

#EXTRA_OECONF += "--without-tcpwrappers"

inherit autotools 

do_configure () {
	oe_runconf ${EXTRA_OECONF}
}

do_install () {
#	oe_runmake install

	install -d ${D}${bindir}
	install -m 0755 tftp/tftp ${D}${bindir}

	install -d ${D}${sbindir}
	install -m 0755 tftpd/tftpd ${D}${sbindir}/in.tftpd

	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${FILESDIR}/init ${D}${sysconfdir}/init.d/tftpd
}

# xinetd will start tftpd automatically
#pkg_postinst_tftp-hpa () {
#if test "x$D" != "x"; then
#	D="-r $D"
#else
#	D="-s"
#fi
#update-rc.d $D tftpd defaults
#}
