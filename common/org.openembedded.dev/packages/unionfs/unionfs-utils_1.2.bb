require unionfs-utils.inc

SRC_URI = "ftp://ftp.fsl.cs.sunysb.edu/pub/unionfs/unionfs-${PV}.tar.gz "

CFLAGS += "-I${S}"

do_compile () {
	oe_runmake -C utils unionctl uniondbg
}

do_install () {
	install -m 0755 -d ${D}${base_sbindir}
	install -m 0755 utils/unionctl utils/uniondbg ${D}${base_sbindir}
}

FILES_${PN} = "${base_sbindir}/unionctl"
FILES_${PN}-dev = "${base_sbindir}/uniondbg"
