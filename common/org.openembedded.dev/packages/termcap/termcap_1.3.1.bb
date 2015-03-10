DESCRIPTION = "GNU termcap library"
HOMEPAGE = "http://www.gnu.org"
LICENSE = "GPL"
#DEPENDS = ""

#WARNING: this recipe assumes you have the timezone compiler present in /usr/sbin/zic

SRC_URI = "ftp://ftp.gnu.org/pub/gnu/termcap/${P}.tar.gz \
			file://install.patch;patch=1 \
		"

inherit autotools pkgconfig

EXTRA_OECONF = "--enable-install-termcap DESTDIR=${D}"

FILES_${PN}-doc += "/usr/info/*"
#FILES_${PN}-doc += "${prefix}/doc/" 



