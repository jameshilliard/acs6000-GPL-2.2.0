DESCRIPTION = "Linux Kernel for Avocent DSI5200"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-${PV}.tar.bz2 \
           file://defconfig \
	   	  "
SRC_URI_append_mergepoint = " file://select-limit.patch;patch=1"

S = "${WORKDIR}/linux-${PV}"

inherit kernel

COMPATIBLE_MACHINE = "dsi5200"
KERNEL_IMAGETYPE = "bzImage"

do_configure_prepend() {
	install -m 0644 ${WORKDIR}/defconfig ${S}/.config
}

