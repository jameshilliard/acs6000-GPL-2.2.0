DESCRIPTION = "librpcsecgss package"
HOMEPAGE = "http://www.citi.umich.edu/projects/nfsv4/linux/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.citi.umich.edu/projects/nfsv4/linux/librpcsecgss/librpcsecgss-${PV}.tar.gz;localpath=${DL_DIR}/librpcsecgss-${PV}.tar.gz"

#SRC_URI += "file://mk.patch;patch=1"

S=${WORKDIR}/librpcsecgss-${PV}

inherit autotools

EXTRA_OECONF += "cross_compiling=yes --prefix=${D} PKG_CONFIG_PATH=${STAGING_LIBDIR}/pkgconfig GSSAPI_CFLAGS=-I${STAGING_INCDIR}"

PACKAGES = "${PN}"

FILES_${PN} = "\
	/usr/lib/* \
	"

do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake
}

do_stage() {
	install -d ${STAGING_LIBDIR}/pkgconfig ${STAGING_INCDIR}/rpc
	install -m 0644 ${S}/include/rpcsecgss/rpc/*.h ${STAGING_INCDIR}/rpc
	install -m 0644 ${S}/src/.libs/librpcsecgss.so ${STAGING_LIBDIR}
	install -m 0644 ${S}/src/.libs/librpcsecgss.a ${STAGING_LIBDIR}
	install -m 0644 ${S}/librpcsecgss.pc ${STAGING_LIBDIR}/pkgconfig
}

do_install() {
	oe_runmake install DESTDIR=${D}
}
