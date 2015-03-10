DESCRIPTION = "Linux Kernel V2.6.20"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

DEPENDS += "u-boot dtc-native"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.23.tar.bz2"
           
SRC_URI_append = "\
		file://linux-fsl-2.6.23-MPC8313ERDB-Fix-TSECIrqNum.patch;patch=1 \
		file://linux-fsl-2.6.23-mpc8xxx-CodeWarrior.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-Fix-Phy-Support.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-Backport-Fixed-Phy.patch;patch=1 \
		file://linux-fsl-2.6.23-MISC_FIX_FOR_GIANFAR_DRIVER.patch;patch=1 \
		file://linux-fsl-2.6.23-GIANFAR_SKB_BUFFER_RECYCLING_SUPPORT-2.patch;patch=1 \
		file://linux-fsl-2.6.23-GIANFAR_PARAMETER_ADJUST.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-Nand.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-JFFS.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC83xx-original-mmc-spi-from-2.6.26.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC83xx-backport-mmc-spi-from-2.6.26-2.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-spi.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-RTC.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-sgmii.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-IEEE-1588.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-PowerManagement.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC831x-LFC.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-Performance-Monitor.patch;patch=1 \
		file://linux-2.6.20-MPC8313ERDB-Marvell.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC8313ERDB-ETSEC27-errata-workaround.patch;patch=1 \
		file://linux-2.6.23-mpc8315erdb-otg-2.patch;patch=1 \
		file://linux-fsl-2.6.23-MPC83xx-SGMII-fix.patch;patch=1 \
		file://linux-fsl-2.6.23-OpenSwan.patch;patch=1 \
		file://linux-fsl-2.6.23-OpenSwan_kernel.patch;patch=1 \
		file://linux-fsl-2.6.23-OCF_Framework.patch;patch=1 \
		file://linux-fsl-2.6.23-mpc837xemds-spin-lock-bugfix.patch;patch=1 \
		file://0007-change-SEC-OCF-driver-to-use-of_platform-and-get-rev.patch;patch=1 \
		file://0008-fsl_usb2_udc-fix-potential-QH-corruption.patch;patch=1 \
		file://0001-gianfar-fix-Lossless-Flow-Control-support-1.patch;patch=1 \
		file://0002-gianfar_1588-fix-bug.patch;patch=1 \
		file://avocent-linux-2.6.23-gsp-sysctl.patch;patch=1 \
		file://avocent-linux-2.6.23-pmhd.patch;patch=1 \
		file://avocent-linux-2.6.23-serial-8250.patch;patch=1 \
		file://avocent-linux-2.6.23-i2c-m41t00.patch;patch=1 \
		file://avocent-linux-2.6.23-marvell-881116.patch;patch=1 \
		file://avocent-linux-2.6.23-brokenness.patch;patch=1 \
		file://RFC4862_5.4.5.patch;patch=1 \
		file://RFC4861_RFC4191.patch;patch=1 \
		file://RFC4291_2.5.3.patch;patch=1 \
		file://autoconf.patch;patch=1 \
		file://linux_null_pointer_dereference_due_to_incorrect_proto_ops.patch;patch=1 \
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

