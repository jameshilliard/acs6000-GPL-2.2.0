require dsi5200-image.bb

export IPKG_INSTALL += " \
	task-caesar-dtview \
	"

IMAGE_ROOTFS_SIZE_ext2.gz = "115200"
