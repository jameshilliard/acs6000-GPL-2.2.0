DESCRIPTION = "Replacement syslog API"
LICENSE = "BSD"
PR = "r0"

SRC_URI = "http://www.balabit.com/downloads/files/syslog-ng/sources/2.0/src/${P}.tar.gz"

inherit autotools pkgconfig

do_stage () {
    install -m 0644 src/evtlog.h src/evtmaps.h ${STAGING_INCDIR}/
    oe_libinstall -a -so -C src/.libs libevtlog ${STAGING_LIBDIR}/
}
