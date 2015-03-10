DESCRIPTION = "Firmware utilities"

DEPENDS += "openssl-native"

inherit native

MODULE_NAME = fw-utils

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"

S=${WORKDIR}/${MODULE_NAME}

CFLAGS_append_acs6k_ppc440 = " -DPRODUCT_TYPE_PMHD=38"
CFLAGS_append_pmhd_mpc8313 = " -DPRODUCT_TYPE_PMHD=43"

do_compile () {
	STAGING_DIR=${STAGING_DIR}/${BUILD_SYS} make all
}

do_stage() {
	install -d ${STAGING_DIR}/${BUILD_SYS}/sbin
	install -m755 checkImage ${STAGING_DIR}/${BUILD_SYS}/sbin
	install -m755 kernelhdr ${STAGING_DIR}/${BUILD_SYS}/sbin
	install -m755 flash2x ${STAGING_DIR}/${BUILD_SYS}/sbin
}

do_install() {
	:
}

