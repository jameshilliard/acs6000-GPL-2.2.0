DESCRIPTION = "Image for ${MACHINE} (${PRODUCT}) "
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "jiff.shen@avocent.com"
PR = "r1"

COMPATIBLE_MACHINE="dsi5200"

RDEPENDS = "${IPKG_INSTALL}"

export IPKG_INSTALL = "\
	task-bootstrap \
	task-caesar-base-depends \
	task-caesar-base \
	task-caesar-settings \
	task-caesar-apps \
	task-caesar-lib-extra \
	task-caesar-connectivity \
	"

IMAGE_LINGUAS = ""
IMAGE_ROOTFS_SIZE_ext2.gz ?= "71680"


inherit image_ipk 
require latest-image.inc

IMAGE_BASENAME="${PRODUCT}"

clean_files() {
	(cd ${IMAGE_ROOTFS}; rm -rf $(find . -name ".debug"); )	
}

rootfs_postproc () {
	echo "${PRODUCT}, build ${PV}-${DATETIME}" > ${IMAGE_ROOTFS}${sysconfdir}/motd
}

ROOTFS_POSTPROCESS_COMMAND = "rootfs_postproc;"
IMAGE_PREPROCESS_COMMAND =+ "clean_files;"

do_latest () {
	oenote ln -sf ${IMAGE_NAME}.rootfs.${IMAGE_FSTYPES} ${DEPLOY_DIR_IMAGE}/${LATEST_IMAGE}
	ln -sf ${IMAGE_NAME}.rootfs.${IMAGE_FSTYPES} ${DEPLOY_DIR_IMAGE}/${LATEST_IMAGE}
}

do_latest[nostamp] = 1

addtask latest after do_rootfs before do_build
