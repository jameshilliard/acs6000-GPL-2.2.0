require dsi5200-image.bb

DESCRIPTION = "Rescue Image for DSI5200 (${PRODUCT}) "

export IPKG_INSTALL = "\
	task-bootstrap \
	task-caesar-base-depends \
	task-caesar-base \
	task-caesar-settings \
	task-caesar-apps \
	"

IMAGE_ROOTFS_SIZE_ext2.gz = "40960"

IMAGE_BASENAME = "${RESCUE_IMAGE_BASENAME}"
LATEST_IMAGE = "${LATEST_IMAGE_RESCUE}"

DEFAULT_SHELL = "/bin/bash"
