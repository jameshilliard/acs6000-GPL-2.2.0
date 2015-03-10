DESCRIPTION = "C-based tyepsafe-api to cacpd."
MODULE_NAME = typesafe-api

# We need cacpd-native so we can run --dump-tree-and-exit, and we also need
# cacpd so we have the CACPD configuration files in place.

IP_DEPENDS="cacpd-native cacpd fm-all"

DEPENDS='glib-2.0 libdaemon llconf-native libcacpdparser gsp-auth ${@base_contains("GSP_FEATURES", "GPL_ENV", "", "${IP_DEPENDS}", d)}'
PROVIDES = "typesafe-api"

EXTRA_CFLAGS = -I${STAGING_INCDIR}/glib-2.0/include -I${STAGING_LIBDIR}/glib-2.0/include
LIB_DIR = ${STAGING_LIBDIR}
SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME};proto=https"

PACKAGES = "${PN}"
ALLOW_EMPTY = 1

S=${WORKDIR}/${MODULE_NAME}

do_compile () {
	make STAGING_ROOT=${STAGING_DIR}/${HOST_SYS} STAGING_INCDIR=${STAGING_INCDIR} ENABLE_GSPAAA=1 all
}

do_stage () {
    oe_libinstall -C build/ libgsp_tsapi ${STAGING_LIBDIR}
    install -m 755 -d ${STAGING_INCDIR}/gsp-tsapi/
    ( cd generated ; for header_file in gsp_tsapi_*.h parameter_types.h type_wrappers.h ; do
	install -m 644 "$header_file" ${STAGING_INCDIR}/gsp-tsapi/
    done )
    install -m 644 src/type_cacpd_client.h ${STAGING_INCDIR}/gsp-tsapi/
}

