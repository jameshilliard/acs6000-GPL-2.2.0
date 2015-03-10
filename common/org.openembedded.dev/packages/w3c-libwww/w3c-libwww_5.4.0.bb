DESCRIPTION="A general-purpose client side WEB API"
HOMEPAGE="http://www.w3.org/Library/"
LICENSE="W3C"

DEPENDS += "zlib openssl expat perl-native"
RDEPENDS += "zlib openssl expat"

SRC_URI="http://www.w3.org/Library/Distribution/${P}.tgz \
	file://libwww-5.4.0-autoconf-2.5.patch;patch=1 \
"
#	file://w3c-libwww-5.4.0.patch;patch=1"

inherit autotools pkgconfig

EXTRA_OECONF = "--enable-shared --with-ssl=${STAGING_LIBDIR}/.. --with-expat"
CFLAGS += "-I${STAGING_INCDIR}/openssl"

FILES_${PN} = "${bindir}/* ${libdir}/lib*.so.*"
FILES_${PN}-doc += "${datadir}/${PN}/*"

