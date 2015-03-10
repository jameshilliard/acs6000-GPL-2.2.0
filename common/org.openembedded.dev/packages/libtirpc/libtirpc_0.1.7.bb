DESCRIPTION = "TI-RPC is an enhanced version of TS-RPC"
AUTHOR = "Gilles Quillard<gilles.quillard@bull.net> Antoine Fraticelli <antoine.fraticelli@bull.net>"
HOMEPAGE = "http://nfsv4.bullopensource.org/doc/tirpc_rpcbind.php"
SECTION = "network"
PRIORITY = "optional"
LICENSE = "GPLv2"
DEPENDS = ""
PR = "r1"

SRC_URI = "svn://libtirpc.svn.sourceforge.net/svnroot/libtirpc/tags/;module=${PV};proto=https;localpath=${DL_DIR}/libtirpc_${PV}.tar.gz \
        file://install.patch;patch=1"

S=${WORKDIR}/${PV}

inherit autotools

CFLAGS_powerpc += "-D__ppc__"

do_stage () {
   oe_runmake DESTDIR=${D} install
   cp -a ${D}/${includedir}/* ${STAGING_INCDIR}/
   cp -a ${D}/${libdir}/* ${STAGING_LIBDIR}/
}
