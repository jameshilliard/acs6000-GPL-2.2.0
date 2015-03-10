DESCRIPTION = "Minimal Perfect Hashing Library"
SECTION = "libs"
AUTHOR = "Davi de Castro Reis <davi@users.sourceforge.net> et al"
HOMEPAGE = "http://cmph.sourceforge.net/"
MAINTAINER = "Oliver Kurth <oliver.kurth@avocent.com>"
LICENSE = "LGPL"

SRC_URI = "http://internap.dl.sourceforge.net/sourceforge/cmph/cmph-0.7.tar.gz"

inherit autotools pkgconfig

do_stage () {
	oe_libinstall -a -so libcmph ${STAGING_LIBDIR}
	for i in cmph.h cmph_types.h; do
		install -m 0644 ${S}/src/$i ${STAGING_INCDIR}/
	done
}
