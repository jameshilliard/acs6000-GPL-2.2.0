inherit pkgconfig

require openssl.inc

PR = "r9"

SRC_URI = "http://www.openssl.org/source/openssl-${PV}.tar"

SRC_URI += "file://mk.patch;patch=1 \
"

PARALLEL_MAKE = ""
