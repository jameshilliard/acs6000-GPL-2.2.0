DESCRIPTION = "Linux Kernel V2.6.24"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

DEPENDS += "u-boot"

SRC_URI = "ftp://ftp.denx.de/pub/linux/denx-linux-${PV}.tar.bz2"
           
SRC_URI_append = "\
                file://add-uImage-creation.patch;patch=1 \
                file://remove-depmod-if-cross.patch;patch=1 \
                file://lm75-detect-tmp100.patch;patch=1 \
                file://add-ppc4xx-m88e1121-support.patch;patch=1 \
                file://rtc-sync-sysclk.patch;patch=1 \
                file://exar_xr16V798.patch;patch=1 \
                file://remove-norflash-log.patch;patch=1 \
                file://gsp-avocent.patch;patch=1 \
                file://acs6000.patch;patch=1 \
                file://serial.patch;patch=1 \
                file://230400bps_8XMODE_fix.patch;patch=1 \
                file://cardbus_danpex_1500st.patch;patch=1 \
                file://cardbus_delkin_cf.patch;patch=1 \
                file://drivers-net-usb.patch;patch=1 \
                file://drivers-usb.patch;patch=1 \
#                file://drivers-wireless.patch;patch=1 \
                file://include-usb.patch;patch=1 \
#                file://include-wireless.patch;patch=1 \
#                file://net-wireless.patch;patch=1 \
                file://usbKconfig.patch;patch=1 \
                file://gc89.patch;patch=1 \
                file://cardbus-xircom.patch;patch=1 \
                file://RFC4862_5.4.5.patch;patch=1 \
                file://halt-acs6000.patch;patch=1 \
                file://usbHController.patch;patch=1 \
                file://rtc-init-m41t00-regs.patch;patch=1 \
                file://CVE-2009-2692.patch;patch=1 \
                file://defconfig \
"

SRC_URI_append_acs6k-oem1 = "\
                file://acs6000-oem1.patch;patch=1 \
"

S = "${WORKDIR}/linux-2.6-denx"

inherit kernel

KERNEL_OUTPUT = "arch/${ARCH}/boot/images/${KERNEL_IMAGETYPE}"
KERNEL_IMAGETYPE = "uImage"

do_configure_prepend() {
    install -m 0644 ${WORKDIR}/defconfig ${S}/.config
}

do_compile_append () {
    ${CC} -Wall -Wstrict-prototypes -O ${TARGET_CPPFLAGS_append} ${S}/Documentation/networking/ifenslave.c -o ${S}/ifenslave 
}

do_stage_append () {
    install -m 0644 arch/ppc/boot/images/vmlinux.gz ${STAGING_KERNEL_DIR}/

    install -d ${STAGING_KERNEL_DIR}/include/asm-powerpc
    cp -fR include/asm-powerpc/* ${STAGING_KERNEL_DIR}/include/asm-powerpc

    install -d ${STAGING_KERNEL_DIR}/arch/ppc/include
    install -d ${STAGING_KERNEL_DIR}/arch/ppc/platforms
    cp -fR arch/ppc/platforms/* ${STAGING_KERNEL_DIR}/arch/ppc/platforms
    rm -f ${STAGING_KERNEL_DIR}/arch/ppc/include/asm
    ln -sf ${STAGING_KERNEL_DIR}/include/asm-powerpc ${STAGING_KERNEL_DIR}/arch/ppc/include/asm
    install -d ${STAGING_KERNEL_DIR}/linux
    install -d ${STAGING_KERNEL_DIR}/net
    cp -fR include/linux/* ${STAGING_KERNEL_DIR}/include/linux
    cp -fR include/net/* ${STAGING_KERNEL_DIR}/include/net
}

do_install_append () {
	install -d ${D}${base_sbindir}
	install -m 0755 ${S}/ifenslave ${D}${base_sbindir}
}

#Added this firmware for wireless lan driver zd1211
#it will be installed in /lib/firmware/zd1211 
FILES += "/sbin/ifenslave" 
