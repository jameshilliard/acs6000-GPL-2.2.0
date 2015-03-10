MAINTAINER="Jiff Shen<jiff.shen@osatechnologies.com.cn>"
SECTION = "db"
DEPENDS = "zlib readline timezone-data-native" 

PR = "r1"

# ------------------------------------------
# NOTE: This package is currently only meant
# to be built nativly on the target device
# ------------------------------------------

SRC_URI = "ftp://ftp2.us.postgresql.org/postgresql/source/v${PV}/postgresql-${PV}.tar.bz2 \
	file://zic-cross.patch;patch=1"

S = "${WORKDIR}/postgresql-${PV}"

inherit autotools

PACKAGES = "postgresql"

prefix="/usr/local"

EXTRA_OECONF = "--prefix=/usr/local/pgsql --with-libs=${STAGING_LIBDIR}"

# comment
pkg_postinst_postgresql() {
if test "x$D" = "x"; then
	addgroup -g 31 postgres || true
	adduser -H -S -D -G postgres -s /bin/false postgres || true
else
	exit 1
fi
}

