DESCRIPTION = "A replacement for libcrypt which comes with GNU C Library"
LICENSE = "GPLv2"

SRC_URI = "${GENTOO_MIRROR}/${P}.tar.gz"
SRC_URI += "file://libxcrypt_2.3-1.patch;patch=1"

DEBIAN_NOAUTONAME_libxcrypt = "1"

inherit autotools

do_stage () {
	install -m 0644 src/xcrypt.h ${STAGING_INCDIR}
	oe_libinstall -C src/.libs libxcrypt ${STAGING_LIBDIR}
}
