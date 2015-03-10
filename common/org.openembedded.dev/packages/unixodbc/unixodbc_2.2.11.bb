DESCRIPTION = "unix ODBC"
SECTION = "dev/libs"
LICENSE = "GPL"
DEPENDS = "flex-native"

SRC_URI = "http://www.unixodbc.org/unixODBC-${PV}.tar.gz"
SRC_URI_append_mergepoint = " file://psql-limit.patch;patch=1"

S = "${WORKDIR}/unixODBC-${PV}"

inherit autotools

EXTRA_OECONF = "--enable-gui=no"
export SED=sed

do_stage () {
	for fn in sql.h sqltypes.h sqlext.h sqlucode.h \
		odbcinst.h odbcinstext.h uodbc_stats.h
	do
		install -m 0644 ${S}/include/$fn ${STAGING_INCDIR}
	done

	oe_libinstall -C DriverManager/.libs libodbc ${STAGING_LIBDIR}
}

