inherit pkgconfig

require openssl.inc

PR = "r9"

SRC_URI += "file://mk.patch;patch=1 \
"

PARALLEL_MAKE = ""
