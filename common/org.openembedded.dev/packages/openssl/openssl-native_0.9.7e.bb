inherit pkgconfig native

require openssl.inc

PR = "r0"

SRC_URI += "file://debian.patch;patch=1 \
            file://armeb.patch;patch=1 \
            file://gnueabi-arm.patch;patch=1 \
            file://md2.patch;patch=1"

FILESPATH = "${@base_set_filespath( ['${FILE_DIRNAME}/openssl-${PV}', '${FILE_DIRNAME}/openssl', '${FILE_DIRNAME}/files', '${FILE_DIRNAME}' ], d)}"

do_install() {
	:
}

do_stage () {
	cp --dereference -R include/openssl ${STAGING_INCDIR}/
	oe_libinstall -a -so libcrypto ${STAGING_LIBDIR}
	oe_libinstall -a -so libssl ${STAGING_LIBDIR}
	install -m755 apps/openssl ${STAGING_BINDIR}
}

