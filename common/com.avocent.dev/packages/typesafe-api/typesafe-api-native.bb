inherit native
require typesafe-api.bb
PROVIDES="typesafe-api-native"
STATIC_DATA_ARCH=powerpc-linux

DEPENDS="libdaemon-native llconf-native libcacpdparser-native cacpd-native"

S="${WORKDIR}/typesafe-api"

CFLAGS_append=" -g -O0"

do_compile () {
	# This is evil, but if we're building a native typesafe API using static data generated
	# from a non-native build, we're already being evil...
	make STAGING_ROOT=${STAGING_DIR}/${STATIC_DATA_ARCH}/ STAGING_INCDIR=${STAGING_INCDIR} ENABLE_GSPAAA= all
}

do_stage_append() {
	# Since we're actually going to *run* CACPD natively if we're building typesafe-api
	# native, we need to make the directory in which CACPD will try to listen.
	install -d ${STAGING_DIR}/${STATIC_DATA_ARCH}/dev/
}
