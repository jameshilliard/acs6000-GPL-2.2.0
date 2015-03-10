DESCRIPTION = "A replacement for libcrypt which comes with GNU C Library"
LICENSE = "GPLv2"

SRC_URI = "${GENTOO_MIRROR}/${P}.tar.bz2"

inherit autotools

do_stage () {
	install -m 0644 src/xcrypt.h ${STAGING_INCDIR}
	oe_libinstall -C src/.libs libxcrypt ${STAGING_LIBDIR}
}
