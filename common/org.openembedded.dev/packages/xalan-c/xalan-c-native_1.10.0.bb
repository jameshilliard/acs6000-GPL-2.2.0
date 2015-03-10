require xalan-c_${PV}.bb

inherit native

CFLAGS += "-I${STAGING_INCDIR}"
CXXFLAGS += "-I${STAGING_INCDIR}"
LDFLAGS += "-L${STAGING_LIBDIR}"

do_configure() {
    ./runConfigure -p linux -c ${BUILD_PREFIX}gcc -x ${BUILD_PREFIX}g++ -r pthread -P "${D}/usr"
	[ -d obj ] || mkdir -p obj lib bin nls/include 
}

do_compile () {
    make -C  ${S}/src/xalanc/Utils/ locale
}

do_stage () {
	:
}

do_install () {
	:
}
