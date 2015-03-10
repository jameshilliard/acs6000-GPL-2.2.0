DESCRIPTION = "Event library"

require libcyevent.inc

inherit avocentgsp 

DEPENDS = "event-notify-headers"

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=libcyevent;rev=209;localpath=${DL_DIR}/libcyevent"

S=${WORKDIR}/libcyevent

PACKAGES = "${PN} "
FILES_${PN} = "${libdir}/*.so* ${bindir}/* ${sysconfdir}/*"

