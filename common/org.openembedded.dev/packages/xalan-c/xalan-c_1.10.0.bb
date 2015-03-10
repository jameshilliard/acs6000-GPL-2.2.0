DESCRIPTION = "XSLT processor for transforming XML into HTML, text, \
or other XML types"
SECTION =  "dev/libs"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
LICENSE = "Apache-1.1"
PR = "r0"

DEPENDS = "xerces-c-native xerces-c"

LEAD_SONAME="libxalan-c.so.*"

SRC_URI = "${APACHE_MIRROR}/xml/${PN}/source/Xalan-C_1_10_0-src.tar.gz"

S="${WORKDIR}/xml-xalan/c"

inherit pkgconfig autotools

PARALLEL_MAKE=""
#
# Warning: BITSTOBUILD will default to 32 bits
#
export XALAN_LOCALE="en_US"
export XALAN_LOCALE_SYSTEM="inmem"
export THREADS="pthread"
export LIBS = " -lpthread "

PARALLEL_MAKE = ""
export XERCESCROOT="${STAGING_INCDIR}"
export XALANCROOT="${S}"

do_configure() {
    ./runConfigure -p linux -c "${BUILD_PREFIX}gcc" -x "${BUILD_PREFIX}g++" -r pthread -P "${D}/usr"
	[ -d obj ] || mkdir -p obj lib bin nls/include 
    make -C  ${S}/src/xalanc/Utils/ locale CXXFLAGS="${BUILD_CXXFLAGS}" CFLAGS="${BUILD_CFLAGS}" LDFLAGS="${BUILD_LDFLAGS}" CPPFLAGS="${BUILD_CPPFLAGS}"

	rm -rf lib obj

    ./runConfigure -p linux -c ${TARGET_PREFIX}gcc -x ${TARGET_PREFIX}g++ -r pthread -P "${D}/usr"
	./configure --host=${HOST_SYS} --target=${TARGET_SYS} --prefix=${prefix}
}

do_stage () {
    oe_libinstall -C lib libxalan-c ${STAGING_LIBDIR}
    oe_libinstall -C lib libxalanMsg ${STAGING_LIBDIR}
	ALL_HEADERS_DIRS="DOMSupport \
					   Include \
					   PlatformSupport \
					   XMLSupport \
					   XPath \
					   XSLT \
					   XalanDOM \
					   XalanEXSLT \
					   XalanExtensions \
					   XalanSourceTree \
					   XercesParserLiaison \
					   XercesParserLiaison/Deprecated \
					   XalanTransformer \
					   Harness \
					   XPathCAPI"

	cd src/xalanc/

	for hdir in $ALL_HEADERS_DIRS; \
	do \
		ddir=${STAGING_INCDIR}/xalanc/$hdir; \
		[ -d $ddir ] || mkdir -p $ddir; \
		fns=$(find $hdir -maxdepth 1 -regex '.*\.h\(pp\)?'); \
		if [ -z "$fns" ]; then : ; else cp -f $fns $ddir/; fi; \
	done
}

