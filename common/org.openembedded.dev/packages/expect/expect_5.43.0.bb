DESCRIPTION = "expect,there is a bug in expect when using interactive ssh,FIXME?"
SECTION = "libs"
LICENSE = "GPL"
DEPENDS = "tcl"
RDEPENDS = "tcl"
SRC_URI = "http://expect.nist.gov/src/expect-${PV}.tar.gz \
    file://without-tk.patch;patch=1 \
    file://cross-compile.patch;patch=1"

S = "${WORKDIR}/expect-5.43"

inherit autotools

PACKAGES =+ "libexpect"

FILES_${PN} = "${bindir}/expect"
FILES_libexpect = "${libdir}/libexpect*.so"
FILES_${PN}-dev = "${includedir}/*.h ${libdir}/libexpect*.a ${libdir}/pkgIndex.tcl"
RDEPENDS_expect += "libexpect"

do_configure () {
	TCL_SRC_DIR=$(sed -nr "s/TCL_SRC_DIR='(.*)'/\1/p" ${STAGING_BINDIR}/tclConfig.sh)/unix
	EXTRA_OECONF="--with-x=no --with-tk=no --enable-shared --with-tcl=$TCL_SRC_DIR expect_cv_long_wnohang=1"
	oenote EXTRA_OECONF ${EXTRA_OECONF}
    if [ ! -e acinclude.m4 -a -e aclocal.m4 ]
    then
        cp aclocal.m4 acinclude.m4
    fi
    autotools_do_configure
}

do_install () {
	oe_runmake INSTALL_ROOT='${D}' LOCAL_EXPECT="expect" install
	cp -f libexpect5.43.* ${D}/${libdir}/expect5.43/
	mv -f ${D}/${libdir}/expect5.43/* ${D}/${libdir}/
}

do_stage () {
        cp -f libexpect5.43.* ${STAGING_LIBDIR}/
}

