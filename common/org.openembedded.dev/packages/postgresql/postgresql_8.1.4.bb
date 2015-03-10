DESCRIPTION = "PostgreSQL is a powerful, open source relational database system."
HOMEPAGE = "http://www.postgresql.com"
LICENSE = "BSD"
DEPENDS = "zlib readline timezone-data-native"

#WARNING: this recipe assumes you have the timezone compiler present in /usr/sbin/zic

SRC_URI = "ftp://ftp.nl.postgresql.org/pub/mirror/postgresql/source/v${PV}/${P}.tar.bz2 \
    file://zic-cross.patch;patch=1 \
    file://prebuilt.patch;patch=1 \
    "


inherit autotools pkgconfig

FILES_${PN}-doc += "${prefix}/doc/" 

pkg_postinst_postgresql() {
if test "x$D" = "x"; then
	addgroup -g 31 postgres || true
	adduser --system --no-create-home --disabled-password --ingroup postgres -s /bin/false postgres
else
	exit 1
fi
}
