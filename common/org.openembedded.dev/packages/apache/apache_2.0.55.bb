MAINTAINER="David Karlstrom <daka@nslu2-linux.org>"
SECTION = "net"
DEPENDS = "openssl expat libpcre apr apr-util"
RDEPENDS += "libssl libcrypto"

PR = "r4"

# ------------------------------------------
# NOTE: This package is currently only meant
# to be built nativly on the target device
# ------------------------------------------

SRC_URI = "http://www.apache.org/dist/httpd/httpd-${PV}.tar.gz \
		file://cross-compile.patch;patch=1 "

S = "${WORKDIR}/httpd-${PV}"

inherit autotools update-rc.d

INITSCRIPT_NAME = "apache"
INITSCRIPT_PARAMS = "defaults 91 20"

CONFFILES_${PN} = "${sysconfdir}/httpd/httpd.conf \
		   ${sysconfdir}/httpd/ssl.conf \
		   ${sysconfdir}/httpd/magic \
		   ${sysconfdir}/httpd/mime.types \
		   ${localstatedir}/www/htdocs/index.html \
		   ${localstatedir}/www/htdocs/apache_pb.gif \
		  "

PACKAGES = "${PN}-dbg ${PN}-dev ${PN}-doc ${PN}-locale ${PN}"

#FILES_libapr = "${libdir}/libapr*.so.0* ${libdir}/apr.exp"
#FILES_libaprutil = "${libdir}/libaprutil*.so.0* ${libdir}/aprutil.exp"

FILES_${PN}-dbg += "${libdir}/httpd/modules/.debug"
FILES_${PN}-dev += "${localstatedir}/www/build" 

FILES_${PN} += "${libdir}/httpd/modules"

APACHE_MODULES = "--enable-access --enable-auth --enable-auth_anon --enable-auth_dbm \
		--enable-auth_digest --enable-include --enable-log_config --enable-env \
		--enable-mime_magic --enable-cern_meta --enable-expires --enable-deflate \
		--enable-headers --enable-usertrack --enable-unique_id --enable-setenvif \
		--enable-mime --enable-dav --enable-davfs --enable-status --enable-autoindex \
		--enable-asis --enable-info --enable-vhost_alias --enable-negotiation \
		--enable-dir --enable-imap --enable-actions --enable-speling --enable-userdir \
		--enable-alias --enable-rewrite --enable-proxy --enable-proxy_ftp \
		--enable-proxy_http --enable-proxy_connect --enable-cache --enable-suexec \
		--enable-disk_cache --enable-file_cache --enable-mem_cache --enable-cgi \
		--enable-cgid --enable-logio "

CFLAGS_append = " -DPATH_MAX=4096"
CFLAGS_prepend = "-I${STAGING_INCDIR}/openssl "
EXTRA_OECONF = "--sysconfdir=${sysconfdir}/httpd \
		--libexecdir=${libdir}/httpd/modules \
		--localstatedir=${localstatedir} \
		--datadir=${localstatedir}/www \
        --enable-ssl \
		--with-ssl=${STAGING_LIBDIR}/.. \
		--with-dbm=sdbm \
		--with-berkeley-db=no \
		--with-gdbm=no \
		--with-ndbm=no \
		--with-apr=${STAGING_BINDIR} \
		--with-apr-util=${STAGING_BINDIR} \
		--enable-mods-shared=all \
		${APACHE_MODULES} \
		"

do_configure () {
	# Looks like rebuilding configure doesn't work, so we are skipping
	# that and are just using the shipped one
	oe_runconf
}

#do_compile_prepend () {
#	(cd ${S}/srclib/pcre; \
#	oe_runmake CC_FOR_BUILD="${BUILD_CC}" CFLAGS_FOR_BUILD="-DLINK_SIZE=2 -I${S}/include" LINK_FOR_BUILD="${BUILD_CC}" )
#}

do_install_append () {
	set -x
	rm -f ${D}/${localstatedir}/www/htdocs/index*
	rm -f ${D}/${localstatedir}/www/htdocs/apache_pb2.*
	rm -f ${D}/${localstatedir}/www/htdocs/apache_pb.png
	install -m 0644 ${FILESDIR}/index.html ${D}/${localstatedir}/www/htdocs/
    rm -rf ${D}/${localstatedir}/www/manual

	install -d ${D}/${sysconfdir}/init.d
	cat ${FILESDIR}/init | \
		sed -e 's,/usr/sbin/,${sbindir}/,g' \
		    -e 's,/usr/bin/,${bindir}/,g' \
		    -e 's,/usr/lib,${libdir}/,g' \
		    -e 's,/etc/,${sysconfdir}/,g' \
		    -e 's,/usr/,${prefix}/,g' > ${D}/${sysconfdir}/init.d/apache
	chmod 755 ${D}/${sysconfdir}/init.d/apache
	
	install -m 0644 ${FILESDIR}/httpd.conf ${D}/${sysconfdir}/httpd/httpd.conf
	
	rm -f ${D}/${libdir}/libexpat.*
}
