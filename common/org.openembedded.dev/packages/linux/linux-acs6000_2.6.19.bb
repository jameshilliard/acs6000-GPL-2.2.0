DESCRIPTION = "Linux Kernel V2.6.19"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

DEPENDS += "u-boot"

SRC_URI = "ftp://ftp.denx.de/pub/linux/denx-linux-${PV}.tar.bz2"
           
SRC_URI_append = "\
                file://add-uImage-creation.patch;patch=1 \
                file://remove-depmod-if-cross.patch;patch=1 \
                file://lm75-detect-tmp100.patch;patch=1 \
                file://bug_ppc4xx_ethtool_noautoneg.patch;patch=1 \
                file://add-ppc4xx-m88e1121-support.patch;patch=1 \
                file://add-bigend-ehci-support.patch;patch=1 \
                file://add-ppc440epx-ehci-support.patch;patch=1 \
                file://bug-m41t11-set-year.patch;patch=1 \
                file://rtc-sync-sysclk.patch;patch=1 \
                file://exar_xr16V798.patch;patch=1 \
                file://remove-norflash-log.patch;patch=1 \
                file://gsp-Kconfig.patch;patch=1 \
                file://gsp-sysctl.patch;patch=1 \
                file://gsp-config.patch;patch=1 \
                file://gsp-ppc440_wdt.patch;patch=1 \
                file://gsp-ppc440_fecmode.patch;patch=1 \
                file://gsp-avocent-header.patch;patch=1 \
                file://goldengate.patch;patch=1 \
                file://cardbus_danpex_1300st.patch;patch=1 \
                file://serial.patch;patch=1 \
                file://cardbus_danpex_1500st.patch;patch=1 \
                file://cardbus_delkin_cf.patch;patch=1 \
                file://230400bps_8XMODE_fix.patch;patch=1 \
                file://defconfig \
                file://zd1211 \
"

S = "${WORKDIR}/linux-${PV}"

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
}

do_install_append () {
	install -d ${D}${base_libdir}/firmware/zd1211
	install -m 0644 ${WORKDIR}/zd1211/* ${D}${base_libdir}/firmware/zd1211
	install -d ${D}${base_sbindir}
	install -m 0755 ${S}/ifenslave ${D}${base_sbindir}
}

#Added this firmware for wireless lan driver zd1211
#it will be installed in /lib/firmware/zd1211 
FILES += "/lib/firmware/zd1211/* /sbin/ifenslave" 
