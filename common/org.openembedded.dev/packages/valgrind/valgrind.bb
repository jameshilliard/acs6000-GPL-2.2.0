MODULE_NAME = valgrind
PV="3.2.3"
VG_ARCH="ppc32-linux"

SRC_URI = "http://www.valgrind.org/downloads/${PN}-${PV}.tar.bz2;localpath=${DL_DIR}/${PN}-${PV}.tar.bz2\
	file://assume-we-have-fp-ops.patch;patch=1\
	file://configure-default-suppressions.patch;patch=1\
	"

PACKAGES += "${PN}-callgrind ${PN}-full"

# We don't want to ship EVERYTHING in the main package -- it's HUGE!
# Just put the essentials for "memcheck" into the main package.

FILES_${PN}="${bindir}/valgrind \
	${libdir}/valgrind/default.supp \
	${libdir}/valgrind/${VG_ARCH}/memcheck \
	${libdir}/valgrind/${VG_ARCH}/vgpreload_core.so \
	${libdir}/valgrind/${VG_ARCH}/vgpreload_memcheck.so\
	"

FILES_${PN}-dbg += "${libdir}/valgrind/${VG_ARCH}/.debug"

FILES_${PN}-callgrind="\
    ${bindir}/callgrind_annotate \
    ${bindir}/callgrind_control \
    ${libdir}/valgrind/${VG_ARCH}/callgrind \
    "

# The "full" package gets everything else (cachegrind, helgrind, none).

FILES_${PN}-full="${bindir} ${libdir}/valgrind"

# The "dev" package gets a few other misc. static libs.

FILES_${PN}-dev += "${libdir}/valgrind/${VG_ARCH}/*.a"

EXTRA_OECONF=" --enable-tls --without-x"

SRC_URI_append_GSP = "file://${MACHINE}.supp"
EXTRA_OECONF_append_GSP = " --with-default-suppressions=${MACHINE}.supp"

do_configure_prepend() {
	[ -f "${WORKDIR}/${MACHINE}.supp" ] && ln -sf "../${MACHINE}.supp" "${S}/${MACHINE}.supp"
}

do_stage() {
	install -d ${STAGING_INCDIR}/valgrind/
	install -m 0644 include/valgrind.h callgrind/callgrind.h memcheck/memcheck.h helgrind/helgrind.h ${STAGING_INCDIR}/valgrind/
}

inherit autotools pkgconfig srclnk
