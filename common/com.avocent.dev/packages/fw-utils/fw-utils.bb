DESCRIPTION = "Firmware utilities"

DEPENDS += "openssl openssl-native"

MODULE_NAME = fw-utils

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME}"

S=${WORKDIR}/${MODULE_NAME}

CFLAGS_append_acs6k_ppc440 = " -DPRODUCT_TYPE_PMHD=38"
CFLAGS_append_pmhd_mpc8313 = " -DPRODUCT_TYPE_PMHD=43"

do_compile () {
	STAGING_DIR=${STAGING_DIR}/${TARGET_SYS} make all
}

do_stage() {
	install -d ${STAGING_INCDIR}/avocent
	install -D include/*.h ${STAGING_INCDIR}/avocent
	install -D oem_product_table.h ${STAGING_INCDIR}/avocent
	install -D privatekey.pem ${STAGING_LOADER_DIR}/privatekey.pem
}

do_install () {
	install -d ${D}${base_prefix}/sbin
	install -m755 checkImage ${D}${base_prefix}/sbin
	rm -f ${D}${base_prefix}/sbin/checkKernel
	ln -s checkImage ${D}${base_prefix}/sbin/checkKernel
	rm -f ${D}${base_prefix}/sbin/checkFileSystem
	ln -s checkImage ${D}${base_prefix}/sbin/checkFileSystem
	rm -f ${D}${base_prefix}/sbin/checkBoot
	ln -s checkImage ${D}${base_prefix}/sbin/checkBoot
	LD_LIBRARY_PATH=${STAGING_DIR}/${BUILD_SYS}/lib:${LD_LIBRARY_PATH} ${STAGING_BINDIR}/openssl rsa -in privatekey.pem -pubout -out pubkey.pem
	install -D pubkey.pem ${D}${base_prefix}/etc/CA/pubkey.pem
	install -m500 avctWDT ${D}${base_prefix}/sbin
}
