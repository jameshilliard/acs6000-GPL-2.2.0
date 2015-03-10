DESCRIPTION = "An sh-compatible command language interpreter."
HOMEPAGE = "http://cnswww.cns.cwru.edu/~chet/bash/bashtop.html"
DEPENDS = "ncurses"
SECTION = "base/shell"
LICENSE = "GPL"
PR = "r7"

SRC_URI = "${GNU_MIRROR}/bash/bash-${PV}.tar.gz \
        file://bash-3.0-fixes.patch;patch=1 \
	file://signames-mipsel.diff;patch=1 \
	file://mkbuiltin.patch;patch=1"

inherit autotools gettext 

#update-alternatives

#ALTERNATIVE_PRIORITY="90"
#ALTERNATIVE_NAME="sh"
#ALTERNATIVE_PATH="/bin/bash"


PARALLEL_MAKE = ""

bindir = "/bin"
sbindir = "/sbin"

FILES_${PN} = "${bindir}/*sh"

EXTRA_OECONF = "--with-ncurses"
export CC_FOR_BUILD = "${BUILD_CC}"

do_configure () {
	gnu-configize
	oe_runconf
}

do_install_append () {
	pushd ${D}${bindir}
	ln -sf bash sh
	popd
}

pkg_postinst () {
if test "x$D" = "x"; then
    touch ${sysconfdir}/shells
	grep -q "bin/bash" ${sysconfdir}/shells || echo /bin/bash >> ${sysconfdir}/shells
	grep -q "bin/sh" ${sysconfdir}/shells || echo /bin/sh >> ${sysconfdir}/shells
else
    exit 1
fi
}
