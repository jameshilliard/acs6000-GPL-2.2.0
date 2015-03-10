DESCRIPTION = "BSD replacement for libreadline"
HOMEPAGE = "http://cvsweb.netbsd.org/bsdweb.cgi/src/lib/libedit/"
SECTION = "libs"
LICENSE="BSD"
DEPENDS = "ncurses"
PR = "r1"

SRC_URI = "ftp://ftp.linux.ee/pub/gentoo/distfiles/distfiles/${PN}-${PV}.tar \
      file://avocent_gsp.patch;patch=1;pnum=0 \
"

S = "${WORKDIR}/${PN}-${PV}"

inherit autotools

do_stage() {
	autotools_stage_all
}

do_install() {
	autotools_do_install
}

FILES_${PN} = "${libdir}/libedit.so*"


