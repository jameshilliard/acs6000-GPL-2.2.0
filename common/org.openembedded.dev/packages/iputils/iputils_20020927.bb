SECTION = "console/network"
DESCRIPTION = "Utilities for the IP protocol, including traceroute6, \
tracepath, tracepath6, ping, ping6 and arping."
SECTION = "console/network"
MAINTAINER = "Jamie Lenehan <lenehan@twibble.org>"
LICENSE ="BSD"
PR = "r4"

SRC_URI = "http://www.tux.org/pub/people/alexey-kuznetsov/ip-routing/iputils-ss020927.tar.gz \
	    file://debian-fixes.patch;patch=1 \
	    file://makefile.patch;patch=1 \
	    file://standard-header-types.patch;patch=1 \
	    file://tracepath6-mtu-opt.patch;patch=1 \
	    file://remove-glibc-headers.patch;patch=1 \
	    file://iputils-retvals.patch;patch=1 \
	    file://glibc-2.4-compat.patch;patch=1"
S = "${WORKDIR}/iputils"

FILES_${PN} = "${base_bindir}/ping*.${PN} \
               ${bindir}/arping \
               ${bindir}/trace* \

FILES_${PN}-doc		= "${mandir}/man8"

do_compile () {
	oe_runmake 'CC=${CC}' \
		   KERNEL_INCLUDE="${STAGING_INCDIR}" \
		   LIBC_INCLUDE="${STAGING_INCDIR}"
}

do_install () {
	install -m 0755 -d ${D}${base_bindir} ${D}${bindir} ${D}${mandir}/man8
	# SUID root programs
	install -m 4555 ping ${D}${base_bindir}/ping.${PN}
	install -m 4555 ping6 ${D}${base_bindir}/ping6.${PN}
	install -m 4555 traceroute6 ${D}${bindir}/
	# Other programgs
	for i in arping tracepath tracepath6; do
	  install -m 0755 $i ${D}${bindir}/
	done
	# Manual pages for things we build packages for
	for i in tracepath.8 traceroute6.8 ping.8 arping.8; do
	  install -m 0644 doc/$i ${D}${mandir}/man8/
	done
}

# Busybox also provides ping and ping6, so use update-alternatives
# Also fixup SUID bit for applications that need it
pkg_postinst_${PN} () {
        chmod 4555 ${base_bindir}/ping.${PN}
        chmod 4555 ${base_bindir}/ping6.${PN}
        chmod 4555 ${bindir}/traceroute6
        update-alternatives --install ${base_bindir}/ping ping ping.${PN} 100
        update-alternatives --install ${base_bindir}/ping6 ping6 ping6.${PN} 100
}
pkg_prerm_${PN} () {
        update-alternatives --remove ping ping.${PN}
        update-alternatives --remove ping6 ping6.${PN}
}

