DESCRIPTION = "Linux Kernel for alterpath mpc8540 machines"
SECTION = "kernel"
LICENSE = "GPL"
PR = "r1"

DEPENDS += "u-boot"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-${PV}.tar.bz2 \
           ftp://ftp.fsl.cs.sunysb.edu/pub/unionfs/unionfs-1.2.tar.gz \
           file://kvmmax.patch;patch=1 \
           file://OnBoardPCI.patch;patch=1 \
           file://mtd.patch;patch=1 \
           file://cyclades-y.patch;patch=1 \
           file://watchdog.patch;patch=1 \
           file://speaker.patch;patch=1 \
           file://misc_drivers.patch;patch=1 \
           file://nattpatch.patch;patch=1 \
           file://openswan.patch;patch=1 \
           file://mkimage.patch;patch=1 \
           file://gianfar.patch;patch=1 \
           file://defconfig"

# before mkimage.patch
#           file://openswan.patch;patch=1 \

S = "${WORKDIR}/linux-${PV}"

inherit kernel

KERNEL_OUTPUT = "arch/${ARCH}/boot/images/${KERNEL_IMAGETYPE}"
KERNEL_IMAGETYPE = "uImage"

python do_patch () {
    bb.build.exec_func('base_do_patch', d)
    bb.build.exec_func ('unionfs_do_patch', d)
}

unionfs_do_patch () {
    (cd ${WORKDIR}/unionfs-1.2; ./patch-kernel.sh ${S})
}

do_configure_prepend() {
    install -m 0644 ${WORKDIR}/defconfig ${S}/.config
}

do_stage_append () {
    install -m 0644 arch/ppc/boot/images/vmlinux.gz ${STAGING_KERNEL_DIR}/
}
