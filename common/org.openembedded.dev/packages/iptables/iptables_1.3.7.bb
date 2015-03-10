DESCRIPTION = "iptables network filtering tools"
HOMEPAGE = "http://www.netfilter.org/"
SECTION = "console/utils"
LICENSE = "GPL"
RRECOMMENDS = "kernel-module-ip-tables kernel-module-iptable-filter"
PR = "r1"



PACKAGES =+ "${PN}-utils"
FILES_${PN}-utils = "${sbindir}/iptables-save ${sbindir}/iptables-restore ${sbindir}/ip6tables-save ${sbindir}/ip6tables-restore"

SRC_URI = "http://www.netfilter.org/files/iptables-${PV}.tar.bz2 file://libiptc.patch;patch=1"

S = "${WORKDIR}/iptables-${PV}"

PARALLEL_MAKE=""

export COPT_FLAGS = "${SELECTED_OPTMIZATION} -rdynamic"
export KERNEL_DIR = "${STAGING_DIR}"

do_compile () {
	unset CFLAGS
	oe_runmake DESTDIR=${D} BINDIR=${sbindir} LIBDIR=${libdir} MANDIR=${mandir} 
	oe_runmake DESTDIR=${D} BINDIR=${sbindir} LIBDIR=${libdir} MANDIR=${mandir} ip6tables-save ip6tables-restore
}

do_install () {
	unset CFLAGS
	oe_runmake DESTDIR=${D} BINDIR=${sbindir} LIBDIR=${libdir} MANDIR=${mandir} install
	install ip6tables-save ${D}${sbindir}
	install ip6tables-restore ${D}${sbindir}

}

do_stage () {
        oe_libinstall -a -C libiptc libiptc ${STAGING_LIBDIR}
	install -d ${STAGING_INCDIR}/libiptc
	install -m 0644 ${S}/include/libiptc/libiptc.h ${STAGING_INCDIR}/libiptc/
	install -m 0644 ${S}/include/libiptc/libip6tc.h ${STAGING_INCDIR}/libiptc/
	install -m 0644 ${S}/include/libiptc/ipt_kernel_headers.h ${STAGING_INCDIR}/libiptc/
}

FILES_${PN}-doc += "${mandir}"

