DESCRIPTION = "U-boot bootloader"
PROVIDES = "virtual/bootloader"
SECTION = "bootloader"
MAINTAINER = "Holger Schurig"
PRIORITY = "optional"
LICENSE = "GPL"
PR = "r1"

DEPENDS = "fw-utils"

SRC_URI = "${SOURCEFORGE_MIRROR}/${PN}/${PN}-${PV}.tar.bz2" 

SRC_URI_append_pmhd_mpc8313 = "\
                file://marvell-88e1116.patch;patch=1 \
                file://gsp-oe-compile.patch;patch=1 \
                file://gsp-remove-cmds.patch;patch=1 \
                file://gsp-mpc83xx-wdt.patch;patch=1 \
                file://gsp-mpc83xx-fecmode.patch;patch=1 \
                file://gsp-fallback.patch;patch=1 \
                file://gsp-serialnb.patch;patch=1 \
                file://gsp-avocent-format.patch;patch=1 \
                file://avocent-pmhd.patch;patch=1 \
                file://pmhd-cold_boot.patch;patch=1 \
                file://mtestbug-30423fix.patch;patch=1 \
                file://pmhd-fix_ddr_speed.patch;patch=1 \
	"

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX} STAGING_DIR=${STAGING_DIR}/${TARGET_SYS}"
TARGET_LDFLAGS = "" 

UBOOT_MACHINE = "avocent-pmhd_config"
UBOOT_IMAGE = "u-boot-${MACHINE}-${PV}-${PR}.bin"
AVOCENT_BOOTCODE_VERSION = ". AVCT-Boot 1.0.3.5"

inherit base

do_compile () {
	oe_runmake ${UBOOT_MACHINE} 
	oe_runmake EXTRAVERSION="${AVOCENT_BOOTCODE_VERSION}" all
}

do_stage() {
	install -d ${STAGING_DIR}/${HOST_SYS}/include
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot/avocent
	install include/environment.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/config.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/command.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/config_cmd_default.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/libfdt.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/gsp-serialnb.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/configs/*.h ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install -m755 tools/mkimage ${STAGING_BINDIR}
	install -d ${STAGING_LOADER_DIR}
	install ${S}/u-boot.bin ${STAGING_LOADER_DIR}
	rm -f ${STAGING_LOADER_DIR}/u-boot.bin.gz
	gzip ${STAGING_LOADER_DIR}/u-boot.bin
}

do_deploy[dirs] = "${S} ${DEPLOY_DIR_IMAGE}"
do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/u-boot.bin ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
}

addtask deploy before do_build after do_compile
