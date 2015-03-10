inherit native
require latest-image.inc

SRCDATE="now"

DEPENDS = "${PRODUCT}-image ${MACHINE}-image-rescue" 

#SRC_URI = "${AVCT_GSP_SVN}/sources/generic/installcd-x86/trunk;module=livecd;rev=HEAD;localpath=${DL_DIR}/installcd-x86-src.tar.gz"
SRC_URI = "${AVCT_GSP_SVN}/sources/generic/installcd-x86/trunk;module=livecd;localpath=${DL_DIR}/installcd-x86-src.tar.gz"

S=${WORKDIR}/livecd

do_compile () {
	cp -L ${DEPLOY_DIR_IMAGE}/${LATEST_IMAGE} rootfs/images/caesar_os.img.gz
	cp -L ${DEPLOY_DIR_IMAGE}/${LATEST_IMAGE_RESCUE} rootfs/images/caesar_rescue.img.gz
	oe_runmake livecd.iso	
}

do_install () {
	:
}

do_stage () {
	:
}

do_deploy () {
	mv livecd.iso ${DEPLOY_DIR_IMAGE}/installcd-${PRODUCT}-${DATETIME}.iso
}

do_compile[nostamp] = 1
do_deploy[nostamp] = 1
do_build[nostamp] = 1

addtask deploy before do_build after do_stage
