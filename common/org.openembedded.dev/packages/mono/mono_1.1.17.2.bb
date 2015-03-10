DESCRIPTION = "Mono runtime and class libraries, a C# compiler/interpreter"
SECTION = "devel"
LICENSE = "GPL LGPL X11"

DEPENDS = "mono-native glib-2.0"

SRC_URI = "http://go-mono.com/sources/mono/mono-${PV}.tar.gz"
SRC_URI_append_dsi5200 = " file://cpu-pentium.patch;patch=1"

S = "${WORKDIR}/mono-${PV}"

inherit autotools pkgconfig

LEAD_SONAME = "libmono.so.0"

EXTRA_OECONF = "--with-tls=pthread"

FILES_${PN}-doc += "${datadir}/libgc-mono"

do_install() {
	oe_runmake 'DESTDIR=${D}' mono_runtime='mint' install
}
