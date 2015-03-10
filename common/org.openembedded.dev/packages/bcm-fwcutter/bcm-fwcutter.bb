SECTION = "Firmware utils"
DESCRIPTION = "Tool to extract FW from .sys files"
LICENSE = "BSD"

SRC_URI = "http://bcm43xx.berlios.de/fwcutter/bcm-fwcutter.tar.gz \
          file://fix-install.patch;patch=1"

S = "${WORKDIR}/bcm-fwcutter"

do_install () {
	oe_runmake "PREFIX=${D}" install
}
