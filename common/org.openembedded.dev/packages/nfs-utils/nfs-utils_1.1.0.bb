DESCRIPTION = "userspace utilities for kernel nfs"
PRIORITY = "optional"
SECTION = "console/network"
MAINTAINER = "syang <steven.yang@avocent.com>"
LICENSE = "GPL"
PR = "r0"

DEPENDS += "libevent tcp-wrappers libnfsidmap e2fsprogs-libs libgssapi librpcsecgss krb5"

SRC_URI = "${SOURCEFORGE_MIRROR}/nfs/nfs-utils-${PV}.tar.gz \
        file://nfsserver \
	" 

SRC_URI += "file://mk.patch;patch=1"

S = ${WORKDIR}/nfs-utils-${PV}/

PARALLEL_MAKE = ""

# Only kernel-module-nfsd is required here (but can be built-in)  - the nfsd module will
# pull in the remainder of the dependencies.
RDEPENDS = "portmap"
RRECOMMENDS = "kernel-module-nfsd"

INITSCRIPT_NAME = "nfsserver"
# The server has no dependencies at the user run levels, so just put
# it in at the default levels.  It must be terminated before the network
# in the shutdown levels, but that works fine.
INITSCRIPT_PARAMS = "defaults"

inherit autotools update-rc.d

EXTRA_OECONF = "--with-statduser=nobody --enable-nfsv3 --with-statedir=/var/lib/nfs cross_compiling=yes PKG_CONFIG_PATH=${STAGING_LIBDIR}/pkgconfig RPCSECGSS_CFLAGS=-I{STAGING_INCDIR}"

do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake 'BUILD=1'
}

INHIBIT_AUTO_STAGE = "1"

do_install() {
	oe_runmake install DESTDIR=${D}

        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${WORKDIR}/nfsserver ${D}${sysconfdir}/init.d/nfsserver

        install -d ${D}${sbindir}
        install -m 0755 ${S}/utils/mountd/mountd ${D}${sbindir}/mountd
        install -m 0755 ${S}/utils/nfsd/nfsd ${D}${sbindir}/nfsd
        install -m 0755 ${S}/utils/statd/statd ${D}${sbindir}/statd
}
