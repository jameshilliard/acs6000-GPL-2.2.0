DESCRIPTION = "U-boot bootloader"
PROVIDES = "virtual/bootloader"
SECTION = "bootloader"
MAINTAINER = "Holger Schurig"
PRIORITY = "optional"
LICENSE = "GPL"
PR = "r1"

DEPENDS = "fw-utils"

SRC_URI = "${SOURCEFORGE_MIRROR}/${PN}/${PN}-${PV}.tar.bz2" 

SRC_URI_append_acs6k_ppc440 = "\
                file://add-88e1121-support.patch;patch=1 \
                file://bug-nand-ecc-smc.patch;patch=1 \
                file://bug-ppc4xx-etherr-lock.patch;patch=1 \
                file://add-ppc4xx-eth-debug.patch;patch=1 \
                file://gsp-oe-compile.patch;patch=1 \
                file://gsp-remove-cmds.patch;patch=1 \
                file://gsp-config.patch;patch=1 \
                file://gsp-ppc4xx-wdt.patch;patch=1 \
                file://gsp-ppc4xx-fecmode.patch;patch=1 \
                file://gsp-fallback.patch;patch=1 \
                file://gsp-avocent-header.patch;patch=1 \
                file://goldengate.patch;patch=1 \
"

SRC_URI_append_acs6k-oem1 = "\
                file://acs6000-oem1.patch;patch=1 \
"

# Intentionally left empty because PMHD HW should use uboot 1.3.1
#SRC_URI_append_pmhd_mpc8313 = ""

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX} STAGING_DIR=${STAGING_DIR}/${TARGET_SYS}"
TARGET_LDFLAGS = "" 

UBOOT_MACHINE ?= "${MACHINE}_config"
UBOOT_MACHINE_acs6000 ?= "golden_gate_config"
UBOOT_MACHINE_pmhd_mpc8313 ?= "${MACHINE}_config"
UBOOT_IMAGE = "u-boot-${MACHINE}-${PV}-${PR}.bin"
AVOCENT_BOOTCODE_VERSION = ". AVCT-Boot 2.0.1.0"
AVOCENT_BOOTCODE_VERSION_acs6k-oem1 = ". FCL-Boot 2.0.1.0"

inherit base

do_compile () {
	oe_runmake ${UBOOT_MACHINE} 
	oe_runmake EXTRAVERSION="${AVOCENT_BOOTCODE_VERSION}" all
}

#temporary to use acs6000 HW
do_compile_pmhd () {
	oe_runmake golden_gate_config
	oe_runmake EXTRAVERSION="${AVOCENT_BOOTCODE_VERSION}" all
}

do_stage() {
	install -d ${STAGING_DIR}/${HOST_SYS}/include
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install include/environment.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/config.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/cmd_confdefs.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/command.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/configs/*.h ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install include/gsp-serialnb.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
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
