DESCRIPTION = "Smith Micro USB Serial Supplemental Driver (SMUSB)"
SECTION = "base"
PRIORITY = "optional"
LICENSE = "GPL"
RDEPENDS = "kernel (${KERNEL_VERSION})"
DEPENDS = "virtual/kernel"

SRC_URI = "${SOURCEFORGE_MIRROR}/smusb-1.2/smusb-1.2.tgz \
   file://verizon-dev.patch;patch=1;pnum=0 \
   "

S = "${WORKDIR}/smusb-1.2/src"

inherit module-base

EXTRA_OEMAKE = " --debug=bvj V=1 KDIR=${STAGING_KERNEL_DIR} DESTDIR=${D}"

python do_patch() {
	bb.build.exec_func('package_do_patch', d)
	bb.build.exec_func('base_do_patch', d)
}

package_do_patch() {
	(cd ${S}; patch -p1 < ../smusb_1.2_2.6.9-2.6.20.patch; )
}

do_compile() {
	oe_runmake
}

pkg_postinst() {
if test "x$D" != "x"; then
       exit 1
else
	rm -f /lib/modules/${KERNEL_VERSION}/kernel/drivers/usb/serial/airprime.ko
	depmod -ae
fi
}

PACKAGES = "${PN}"
FILES_${PN} = "/lib/modules/"
