DESCRIPTION = "Command line tool and library for client-side URL transfers."
LICENSE = "MIT"
SECTION = "console/network"

SRC_URI = "http://curl.haxx.se/download/curl-${PV}.tar.gz \
           file://pkgconfig_fix.patch;patch=1 \
           file://avct-fix-ld-library-path.patch;patch=1"
S = "${WORKDIR}/curl-${PV}"

#
# CURL_FEATURES can be: zlib, gnutls, cookies, crypto-auth, ipv6, ssl
#                       dict, file, ftp, http, telnet, tftp, and ares,
#
CURL_FEATURES ?= "zlib,cookies,crypto-auth,dict,file,ftp,http,telnet,tftp,ares,ipv6,ssl"

inherit autotools pkgconfig binconfig

EXTRA_OECONF = " \
		--without-libssh2 \
		--with-random=/dev/urandom \
		--without-libidn \
		"

python __anonymous() {
	f = bb.data.getVar("CURL_FEATURES", d, True).split(",")
	oeconf = bb.data.getVar("EXTRA_OECONF", d, False)
	deps = bb.data.getVar("DEPENDS", d, False).split()
	for x in [ 'cookies', 'crypto-auth', 'dict', 'file', 'ftp', 'http', 'telnet', 'tftp', 'ipv6' ]:
		if x in f:
			oeconf += " --enable-%s" % x
		else:
			oeconf += " --disable-%s" % x
	if 'ssl' in f:
		oeconf += " --with-ssl=${STAGING_LIBDIR}/../"
		deps.append("openssl")
	else:
		oeconf += " --without-ssl"
	if 'zlib' in f:
		oeconf += " --with-zlib=${STAGING_LIBDIR}/../"
		deps.append("zlib")
	else:
		oeconf += " --without-zlib"
	if 'gnutls' in f:
		oeconf += " --with-gnutls=${STAGING_BINDIR_CROSS}"
		deps.append("gnutls")
	else:
		oeconf += " --without-gnutls"
	if 'ares' in f:
		oeconf += " --enable-ares"
		deps.append("c-ares")
	else:
		oeconf += " --disable-ares"
	bb.data.setVar('EXTRA_OECONF', oeconf, d)
	bb.data.setVar('DEPENDS', " ".join(deps), d)
}

do_configure_prepend() {
	sed -i s:OPT_GNUTLS/bin:OPT_GNUTLS:g configure.ac
}

do_stage () {
	install -d ${STAGING_INCDIR}/curl
	install -m 0644 ${S}/include/curl/*.h ${STAGING_INCDIR}/curl/
	oe_libinstall -so -a -C lib libcurl ${STAGING_LIBDIR}
}

PACKAGES += "${PN}-certs libcurl libcurl-dev libcurl-doc"

FILES_${PN} = "${bindir}/curl"

FILES_${PN}-certs = "${datadir}/curl/curl-*"
PACKAGE_ARCH_${PN}-certs = "all"

FILES_${PN}-doc = "${mandir}/man1/curl.1"

FILES_lib${PN} = "${libdir}/lib*.so.*"
RRECOMMENDS_lib${PN} += "${PN}-certs"
FILES_lib${PN}-dev = "${includedir} \
                      ${libdir}/lib*.so \
                      ${libdir}/lib*.a \
                      ${libdir}/lib*.la \
                      ${libdir}/pkgconfig \
                      ${datadir}/aclocal \
                      ${bindir}/*-config"

FILES_lib${PN}-doc = "${mandir}/man3 \
                      ${mandir}/man1/curl-config.1"

