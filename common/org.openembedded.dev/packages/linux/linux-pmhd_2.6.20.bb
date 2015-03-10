DESCRIPTION = "Linux Kernel V2.6.20"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

DEPENDS += "u-boot dtc-native"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.20.tar.bz2"
           
SRC_URI_append = "\
                file://linux-2.6.20-MPC8313ERDB-REVA1-2-INIT.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-phylib-modification.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-SGMII.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-SKBUFF-recycling.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-USB.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-oprofile.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-Power-Management.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-IEEE-1588.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-OCF-framework.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-OCF-SEC2.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-Openswan-IPsec-1.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-USB-OTG.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-DTS.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-defconfig.patch;patch=1 \
                file://linux-2.6.20-gianfar-coalescing-fixes.patch;patch=1 \
                file://linux-2.6.20-gianfar-tweaks.patch;patch=1 \
                file://linux-2.6.20-mpc8313-etsec27-errata-workaround.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-IEEE-1588-update.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-Power-Management-update.patch;patch=1 \
                file://linux-2.6.20-mpc8313-rtc-fixup-20070629.patch;patch=1 \
                file://linux-2.6.20-MPC8313ERDB-Marvell.patch;patch=1 \
                file://marvell_88e1116.patch;patch=1 \
                file://m41t00_crash.patch;patch=1 \
                file://rtc-sync-sysclk.patch;patch=1 \
                file://8250_rs485.patch;patch=1 \
                file://gsp-powerpc-Kconfig.patch;patch=1 \
                file://gsp-powerpc-sysctl.patch;patch=1 \
                file://gsp-powerpc-wdt.patch;patch=1 \
                file://gsp-powerpc-ethmode.patch;patch=1 \
                file://avocent-pmhd.patch;patch=1 \
                file://crc-errors.patch;patch=1 \
                file://shutdown-bug51589.patch;patch=1 \
                file://pmhd-log_kernel_panic.patch;patch=1 \
                file://defconfig \
"

S = "${WORKDIR}/linux-${PV}"

inherit kernel

KERNEL_OUTPUT = "arch/powerpc/boot/${KERNEL_IMAGETYPE}"
KERNEL_IMAGETYPE = "uImage"

do_configure_prepend() {
    ARCH=powerpc
    install -m 0644 ${WORKDIR}/defconfig ${S}/.config
}

do_compile_prepend() {
    ARCH=powerpc
}

do_compile_append() {
    ${STAGING_DIR}/${BUILD_SYS}/bin/dtc -R 4 -S 0x3000 -I dts -O dtb -o board.dtb arch/powerpc/boot/dts/avocent-pmhd.dts
}

do_install_prepend() {
    ARCH=powerpc
}

do_stage_append () {
    install -m 0644 vmlinux.bin.gz ${STAGING_KERNEL_DIR}/
    install -m 0644 board.dtb ${STAGING_KERNEL_DIR}/
}

do_stage_append_pmhd () {
    install -m 0644 ${STAGING_KERNEL_DIR}/include/linux/avocent_pmhd_*.h ${STAGING_DIR}/${TARGET_SYS}/include/linux/
}

