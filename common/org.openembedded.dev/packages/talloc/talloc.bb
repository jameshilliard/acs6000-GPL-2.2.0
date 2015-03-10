DESCRIPTION = "hierarchical pool based memory allocator with destructors"
SECTION = "libs"
PRIORITY = "optional"
MAINTAINER = "Andrew Tridgell <talloc@tridgell.net>"
HOMEPAGE = "http://talloc.samba.org/"
LICENSE = "zlib"
PR="r1"

# does not work from build server:
#SRC_URI = "svn://svnanon.samba.org/samba/branches/SAMBA_4_0/source/lib/talloc;module=talloc"

# this version requites libreplace.m4 from the sama sources:
#SRC_URI = "http://wiki.cyclades.com/~oku/download/talloc-svn23252.tar.gz"

# version used on onboard:
SRC_URI = "http://wiki.cyclades.com/~oku/download/talloc-svn13648.tar.bz2"

S = "${WORKDIR}/${PN}"

DEPENDS = "libtool-cross"

inherit autotools

#EXTRA_OECONF = "--prefix=${D}/usr --exec-prefix=${D}/usr"

do_configure_prepend() {
	sh ./autogen.sh
}

do_stage () {
	oe_libinstall -so libtalloc ${STAGING_LIBDIR}
	install -m 0644 ${S}/talloc.h ${STAGING_INCDIR}/
}

do_install() {
        install -d ${D}${prefix} ${D}${includedir} ${D}${libdir}
        oe_runmake "prefix=${D}${prefix}" \
                   "exec_prefix=${D}${exec_prefix}" \
                   "man3dir=${D}${mandir}/man3" \
                   "includedir=${D}${includedir}" \
                   "libdir=${D}${libdir}" install
}

PACKAGES = "${PN} "
FILES_${PN} = "${libdir}/*.so"

