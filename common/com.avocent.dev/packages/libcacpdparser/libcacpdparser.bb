MODULE_NAME = libcacpdparser

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"

DEPENDS="libdaemon dlog glib-2.0 gsp-auth"

DEBIAN_NOAUTONAME_libcacpdparser = "1"

S="${WORKDIR}/${PN}"

EXTRA_OECONF = "--enable-eval-command"

inherit autotools pkgconfig srclnk

do_stage() {
	pwd
	install -d ${STAGING_INCDIR}/libcacpdparser
	install -m 0644 include/libcacpdparser/*.h ${STAGING_INCDIR}/libcacpdparser/
	oe_libinstall -C src/.libs libcacpdparser ${STAGING_LIBDIR}
}
