DESCRIPTION = "U-boot bootloader"
PROVIDES = "virtual/bootloader"
SECTION = "bootloader"
MAINTAINER = "Holger Schurig"
PRIORITY = "optional"
LICENSE = "GPL"
CURRENT_UBOOT_REVISION ?= "r2"
PR = "${CURRENT_UBOOT_REVISION}"


PARALLEL_MAKE=""

SRC_URI = "${SOURCEFORGE_MIRROR}/${PN}/${PN}-${PV}.tar.bz2 "

SRC_URI_append_arm = "file://arm_flags.patch;patch=1 "
SRC_URI_append_vibren = "ftp://bec-systems.com/pub/pxa255_idp/u-boot/uboot_pxa255-idp_2005-03-23.patch;patch=1"
SRC_URI_append_mnci   = "file://mnci.patch;patch=1 \
                         file://mnci-jffs2.patch;patch=1 \
                         file://cmd-arm-linux.patch;patch=1 \
                         file://command-names.patch;patch=1"

SRC_URI_append_alterpath  = " file://cyclades.patch;patch=1"
SRC_URI_append_amazon  = "file://u-boot-1.1.2-huntsville.patch;patch=1 \
							file://amazon-ide.patch;patch=1 \
							file://make-3.81.patch;patch=1 \
							file://oe-compile.patch;patch=1 \
							file://oe-flash.patch;patch=1 \
							file://amazon-menu.patch;patch=1 \
							"

# TODO: SRC_URI_append_rt3000

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX}"
TARGET_LDFLAGS = ""

UBOOT_MACHINE ?= "${MACHINE}_config"
UBOOT_MACHINE_mnci   = "mnci_config"
UBOOT_MACHINE_vibren = "pxa255_idp_config"
UBOOT_MACHINE_alterpath = "DEBV1_config"
UBOOT_MACHINE_amazon = "AMAZON_config"


UBOOT_IMAGE = "u-boot-${MACHINE}-${PV}-${PR}.bin"

inherit base

do_compile () {
	oe_runmake ${UBOOT_MACHINE}
        oe_runmake --version
	oe_runmake 
}

do_compile_amazon () {
#		( echo "#define CONFIG_IDENT_STRING \" Amazon: ${PV}-${PR}\" " >> ${S}/include/configs/AMAZON.h )
		( sed -i 's/#define CONFIG_IDENT_STRING      " Amazon Version Not Set"/#define CONFIG_IDENT_STRING " Amazon: ${PV}-${PR} "/' ${S}/include/configs/AMAZON.h )
	oe_runmake ${UBOOT_MACHINE}
        oe_runmake --version
	oe_runmake 
}

do_stage() {
	install -d ${STAGING_DIR}/${HOST_SYS}/include
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install -d ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install include/environment.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/config.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/cmd_confdefs.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	install include/command.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	if [ -e include/cyc_ide_cfg.h ]; then
		install include/cyc_ide_cfg.h ${STAGING_DIR}/${HOST_SYS}/include/uboot
	fi
	install include/configs/*.h ${STAGING_DIR}/${HOST_SYS}/include/uboot/configs
	install -m755 tools/mkimage ${STAGING_BINDIR}
}

do_deploy () {
	install -d ${DEPLOY_DIR_IMAGE}
	install ${S}/u-boot.bin ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
	install ${S}/u-boot.bin ${DEPLOY_DIR_IMAGE}/u-boot.bin
}
do_deploy[dirs] = "${S}"
addtask deploy before do_build after do_compile


#########################################################

RDEPENDS_append_mnci = " hwctrl"

FILES_${PN}_mnci = "/tmp/${UBOOT_IMAGE}"
do_install_openmn() {
	install -d ${D}/tmp
	install ${S}/u-boot.bin ${D}/tmp/${UBOOT_IMAGE}
}

pkg_postinst_mnci() {
ldconfig
A=/tmp/bootargs
hwctrl kernel_conf_get bootargs >$A
cp /tmp/${UBOOT_IMAGE} /dev/mtdblock/0
rm /tmp/${UBOOT_IMAGE}
hwctrl kernel_conf_set bootargs "`cat $A`"
cat /dev/mtdblock/0 >/dev/null
exit 0
}
