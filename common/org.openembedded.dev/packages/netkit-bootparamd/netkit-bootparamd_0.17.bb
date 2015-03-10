DESCRIPTION="Netkit - bootp"
HOMEPAGE="ftp://ftp.uk.linux.org/pub/linux/Networking/netkit/"
LICENSE="BSD"

PR="r1"

SRC_URI="ftp://ftp.uk.linux.org/pub/linux/Networking/netkit/${P}.tar.gz \
	file://cyclades_changes.diff;patch=1 \
	file://netkit-bootparamd_0.17-7.diff;patch=1 \
	"
inherit autotools

do_configure () {
	export MANDIR=${mandir}
	./configure --installroot=${D} --with-c-compiler="${CC}"
}

do_install_prepend () {
	install -d ${D}${bindir}
	install -d ${D}${sbindir}

	install -d ${D}${mandir}/man8
}
