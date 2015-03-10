DESCRIPTION = "srp 2.1.2 package"
HOMEPAGE = ""
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://srp.stanford.edu/source/srp-${PV}.tar.gz;localpath=${DL_DIR}/srp-${PV}.tar.gz"

SRC_URI += " file://mk.patch;patch=1"

S=${WORKDIR}/srp-${PV}

inherit autotools

EXTRA_OECONF += "cross_compiling=yes --prefix=${D}/usr --exec_prefix=${D}/usr"

PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

export LDLIBS="-L${STAGING_LIBDIR}"

FILES_${PN} = "\
	/usr/bin/* \
	/usr/sbin/* \
	/usr/lib/* \
	"

FILES_${PN}_acs6000 = "\
	"

do_configure() {
	oe_runconf
}

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake install DESTDIR=${D}
}

do_install_acs6000() {
}

do_stage_acs6000() {
	install -m 0644 libsrp/*.a ${STAGING_LIBDIR}
	install -m 0644 libsrp/*.h ${STAGING_INCDIR}
}

do_stage() {
	install -d ${STAGING_LIBDIR}/security
	install -m 0644 ${WORKDIR}/install/srp/usr/lib/*.a ${STAGING_LIBDIR}
	install -m 0644 ${WORKDIR}/install/srp/usr/lib/security/*.so ${STAGING_LIBDIR}/security
	install -m 0644 ${WORKDIR}/install/srp-dev/usr/include/*.h ${STAGING_INCDIR}
}
