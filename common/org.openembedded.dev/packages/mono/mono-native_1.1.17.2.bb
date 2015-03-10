SECTION = "devel"
DEPENDS = "glib-2.0-native"
LICENSE = "GPL LGPL X11"
SRC_URI = "http://go-mono.com/sources/mono/mono-${PV}.tar.gz "

inherit autotools native 

S = "${WORKDIR}/mono-${PV}"

do_compile_prepend () {
	ln -sf ${BUILD_SYS}-libtool libtool
}
