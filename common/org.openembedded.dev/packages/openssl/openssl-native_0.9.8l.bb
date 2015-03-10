inherit pkgconfig native

DEFAULT_PREFERENCE = "-1"

require openssl.inc

PR = "r1"

# This flag can contain target options (e.g -mfpu=neon for armv7-a systems)
export FULL_OPTIMIZATION = " "
export BUILD_OPTIMIZATION = " "

SRC_URI = "http://www.openssl.org/source/openssl-${PV}.tar"

SRC_URI += " file://mk.patch;patch=1 \
"

PARALLEL_MAKE = ""

FILESPATH = "${@base_set_filespath( ['${FILE_DIRNAME}/openssl-${PV}', '${FILE_DIRNAME}/openssl', '${FILE_DIRNAME}/files', '${FILE_DIRNAME}' ], d)}"

do_install() {
	install -m755 apps/openssl ${STAGING_BINDIR}
}

