HOMEPAGE = "http://fuse.sf.net"
DESCRIPTION = "With FUSE it is possible to implement a fully functional filesystem in a userspace program"
MAINTAINER = "Koen Kooi <koen@linuxtogo.org>"

LICENSE = "GPL"


DEPENDS = "fakeroot-native"
RRECOMMENDS = "fuse"

PR = "r1"

SRC_URI="${SOURCEFORGE_MIRROR}/fuse/fuse-${PV}.tar.gz"
S = "${WORKDIR}/fuse-${PV}"

inherit autotools pkgconfig module
EXTRA_OECONF = "  --with-kernel=${STAGING_KERNEL_DIR}"

do_configure() {
cd ${S} ; oe_runconf
}

do_compile(){
LDFLAGS=""
cd ${S}/kernel
oe_runmake
}

fakeroot do_install() {
LDFLAGS=""
install -d ${D}${sysconfdir}/udev/rules.d/
install -m 644 util/udev.rules ${D}${sysconfdir}/udev/rules.d/
cd ${S}/kernel
oe_runmake install DESTDIR=${D}
}

FILES_${PN} = "/dev ${base_libdir}/modules ${sysconfdir}"
