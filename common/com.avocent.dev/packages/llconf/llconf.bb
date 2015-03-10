MODULE_NAME = llconf

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"

S="${WORKDIR}/${PN}"

CFLAGS_append = " -Wall -Werror"

inherit autotools pkgconfig srclnk

do_stage () {
	install -d ${STAGING_INCDIR}/llconf
	install -m 0644 src/*.h ${STAGING_INCDIR}/llconf
	install -m 0644 src/parsers/*.h ${STAGING_INCDIR}/llconf
	oe_libinstall -C src/.libs libllconf ${STAGING_LIBDIR}
}
