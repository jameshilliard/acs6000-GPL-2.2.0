DESCRIPTION = "Cardbus firmware Container for all supported cards"

MODULE_NAME = "devices-fw"

DEPENDS = ""
RDEPENDS = ""

SRC_URI = "file://rt73.bin"

do_compile () {
}

do_install () {
  install -d ${D}/lib/firmware
  install -d ${D}/usr/local/lib/firmware
  install ${WORKDIR}/rt73.bin ${D}/lib/firmware
}

FILES_${PN} = "/lib/firmware/* /usr/local/lib/firmware/*"

