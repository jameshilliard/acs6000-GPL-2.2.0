DESCRIPTION = "Avocent ACS 6000 Linux distribution"
HOMEPAGE = "http://www.avocent.com"
MAINTAINER = "teste"
PR = "r1"
FWVERSION = "1.1.0.0"
BOOTVERSION = "1.0.0.0"
PLUGINVERSION = "1.0.0"
FWPARTNUMBERS = "000-000"
FWOEMS = "Avocent"
FWFAMILY = "ACS6000"
FWCCODE = "us"
FWLCODE = "eng"
FWCHECKSUM = "0"

RDEPENDS = "${IPKG_INSTALL}"

export IPKG_INSTALL = "\
	task-bootstrap \
	task-acs-base-depends \
	task-acs-base \
	task-gsp-apps \
	task-acs-fs \
	task-acs-apps \
	task-acs-connectivity \
	task-acs-utils \
	"

IMAGE_LINGUAS = ""

