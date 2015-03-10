DESCRIPTION = "Lsof is a Unix-specific diagnostic tool. \
Its name stands for LiSt Open Files, and it does just that."
SECTION = "devel"
LICENSE = "BSD"
PR = "r0"

SRC_URI = "ftp://lsof.itap.purdue.edu/pub/tools/unix/lsof/OLD/lsof_${PV}.tar.gz"
S = "${WORKDIR}/lsof_${PV}_src"

LSOF_OS = "${TARGET_OS}"
LSOF_OS_linux-uclibc = "linux"
LSOF_OS_linux-gnueabi = "linux"

do_configure () {
	./Configure ${LSOF_OS}
}

export LSOF_INCLUDE=${STAGING_INCDIR}
export I = "${STAGING_INCDIR}"
export L = "${STAGING_INCDIR}"
export EXTRA_OEMAKE = ""

do_compile () {
	oe_runmake 'CC=${CC}' 'CFGL=${LDFLAGS} -L./lib -llsof' 'DEBUG=' 'INCL=${CFLAGS}'
}

do_install () {
	install -d ${D}${sbindir} ${D}${mandir}/man8
	install -m 4755 lsof ${D}${sbindir}/lsof
	install -m 0644 lsof.8 ${D}${mandir}/man8/lsof.8
}
