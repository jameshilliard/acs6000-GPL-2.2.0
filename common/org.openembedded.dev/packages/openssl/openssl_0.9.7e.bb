require openssl.inc

PR = "r3"

SRC_URI += "file://debian.patch;patch=1 \
            file://armeb.patch;patch=1 \
            file://gnueabi-arm.patch;patch=1 \
            file://md2.patch;patch=1"
