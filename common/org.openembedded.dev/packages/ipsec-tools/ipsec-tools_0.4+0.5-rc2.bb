require ipsec-tools.inc

BV = "${@bb.data.getVar('PV', d, 1).split('+')[1].replace('-','')}"
SRC_URI = "${SOURCEFORGE_MIRROR}/ipsec-tools/ipsec-tools-${BV}.tar.bz2 \
	file://cross.patch;patch=1"

SRC_URI +=" file://conf_ac.patch;patch=1 \
	file://conf.patch;patch=1 \
	file://warning.patch;patch=1 \
	"

S = "${WORKDIR}/ipsec-tools-${BV}"

EXTRA_OECONF += " cross_compiling=yes ac_cv_va_copy=yes"
