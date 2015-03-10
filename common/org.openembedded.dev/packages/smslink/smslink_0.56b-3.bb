DESCRIPTION="Program implements a client/server gateway to the SMS protocol"
HOMEPAGE="http://smslink.sourceforge.net"
LICENSE="GPL-2"

SRC_URI="${SOURCEFORGE_MIRROR}/${PN}/${P}.tar.gz \
	file://client-sendsms.patch;patch=1 \
	file://cyclades-make.diff;patch=1 \
"

do_compile () {
	make PREFIX=${D}${prefix} CC="${CC}" -C client all install
}

do_install () {
	:
}
