DESCRIPTION = "ISDN4Linux Utils"
HOMEPAGE = "http://www.isdn4linux.de"
SECTION = "network"
LICENSE = "GPLv2"
DEPENDS = "virtual/kernel"
PR = "r1"

SRC_URI = "ftp://ftp.isdn4linux.de/pub/isdn4linux/utils/${PN}.v${PV}.tar.bz2 \
        file://cyclades_code_changes_v32p1.diff;patch=1 \
        file://cyclades_config_changes.diff;patch=1 \
        file://cyclades_make_changes.diff;patch=1 \
        file://lib-libtools.patch;patch=1 \
        "
S = ${WORKDIR}/${PN}

inherit module-base

EXTRA_OEMAKE += "CONFIGURE_PARMS=\'--host=${HOST_SYS}\' DESTDIR=${D}"

do_configure () {
    ln -sf .config.rpm .config
    sed -ir "s,CONFIG_KERNELDIR=.*,CONFIG_KERNELDIR=\"${KERNEL_SOURCE}\"," .config
    oe_runmake subconfig
}

do_compile () {
    oe_runmake -C lib
    oe_runmake -C isdnctrl
    oe_runmake -C ipppd
}

fakeroot do_install () {
    oe_runmake -C lib install
    oe_runmake -C isdnctrl install
    oe_runmake -C ipppd install
}

FILES_${PN}-doc += "${prefix}/man"
