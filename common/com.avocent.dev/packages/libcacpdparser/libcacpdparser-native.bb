require libcacpdparser.bb
inherit native

S="${WORKDIR}/libcacpdparser"
CFLAGS_append=" -g -O0"
EXTRA_OECONF="CACPD_SOCKET_PATH=${STAGING_DIR}/powerpc-linux/dev/cacpd"
