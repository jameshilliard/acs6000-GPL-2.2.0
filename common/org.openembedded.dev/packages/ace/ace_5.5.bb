DESCRIPTION = "ADAPTIVE Communication Environment (ACE)"
HOMEPAGE = "http://www.cs.wustl.edu/~schmidt/ACE.html"
SECTION = "lib"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
LICENSE = "ACE"
DEPENDS = "openssl"

LEAD_SONAME = "libACE.so*"

SRC_URI = "http://download.dre.vanderbilt.edu/previous_versions/ACE-${PV}.tar.gz \
        file://configure.patch;patch=1 \
        file://GNUmakefile "

S = "${WORKDIR}/ACE_wrappers"

inherit autotools pkgconfig

EXTRA_OECONF = "--without-x --without-tcl --without-tk --enable-static=yes"
EXTRA_OEMAKE = "-f Makefile"
export ACE_ROOT="${S}"

do_configure_prepend () {
        ln -sf platform_linux.GNU include/makeinclude/platform_macros.GNU
        sed -i '/gperf/d' configure.ac
        sed -i '/\/examples\//d' configure.ac
        sed -i '/\/tests\//d' configure.ac
}

do_configure_append () {
        rm $(find . -name "GNUmakefile")
        for fn in $(find . -regex '.*\(examples\|tests\)'); do
                [ -d $fn ] && cp -f ${WORKDIR}/GNUmakefile $fn/
        done
        cp ${WORKDIR}/GNUmakefile apps/gperf/
}

do_compile () {
        oe_runmake -f Makefile all
}

do_install () {
        oe_runmake DESTDIR=${D} install
        cp -a ${D}${libdir} ${D}${includedir} ${STAGING_DIR}/${HOST_SYS}
}

do_stage () {
       oe_runmake DESTDIR=${D} install
       cp -a ${D}${libdir} ${D}${includedir} ${STAGING_DIR}/${HOST_SYS}
       rm -rf ${D}/*
}

python do_package() {
        if bb.data.getVar('DEBIAN_NAMES', d, 1):
            bb.data.setVar('PKG_${PN}', 'libace5', d)
        bb.build.exec_func('package_do_package', d)
}
