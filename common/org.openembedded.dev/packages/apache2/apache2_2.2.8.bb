SECTION = "net"
DESCRIPTION = "The apache v2 web server"
DEPENDS = "openssl expat apache2-native libpcre "
RDEPENDS += "openssl"

PR = "r5"

SRC_URI = "http://apache.mirrors.tds.net/httpd/httpd-2.2.8.tar.bz2 \
	   file://mod_ssl-patch;patch=1 \
	   file://dftables-makefile-patch;patch=1 \
	   file://apr-sockets-patch;patch=1 \
	   file://configure-patch;patch=1 \
	   file://server-makefile-patch;patch=1 \
           file://configure-fix-cross-compile-ptr-check.patch;patch=1 \
           file://fix_cross_complie_TCP.patch;patch=1 \
	   file://fix_cross_complie_prt_void_length.patch;patch=1 \
	   file://httpd-2.2.8-ssl-io-buffer.patch;patch=1"
#
# over-ride needed since apache unpacks into httpd
#
S = "${WORKDIR}/httpd-${PV}"

#
# implications - autotools defines suitable do_configure, do_install, etc.
# update-rc.d adds hooks for rc-update.
#
#
inherit autotools update-rc.d

#
# implications - used by update-rc.d scripts
#
INITSCRIPT_NAME = "apache2"
INITSCRIPT_PARAMS = "defaults 91 20"
LEAD_SONAME = "libapr-1.so.0"

CONFFILES_${PN} = "${sysconfdir}/${PN}/httpd.conf \
		   ${sysconfdir}/${PN}/magic \
		   ${sysconfdir}/${PN}/mime.types \
		   ${sysconfdir}/init.d/${PN} "

#
PACKAGES = "${PN}-doc ${PN}-dev ${PN}-dbg ${PN}"

# we override here rather than append so that .so links are
# included in the runtime package rather than here (-dev)
# and to get build, icons, error into the -dev package
FILES_${PN}-dev = "${datadir}/${PN}/build \
		${datadir}/${PN}/icons \
		${datadir}/${PN}/error \
		${bindir}/apr-config ${bindir}/apu-config \
		${libdir}/apr*.exp \
		${includedir}/${PN} \
		${libdir}/*.la \
		${libdir}/*.a"

# manual to manual
FILES_${PN}-doc += " ${datadir}/${PN}/manual"

#
# override this too - here is the default, less datadir
#
FILES_${PN} =  "${bindir} ${sbindir} ${libexecdir} ${libdir}/lib*.so.* ${sysconfdir} \
		${sharedstatedir} ${localstatedir} /bin /sbin /lib/*.so* \
		${libdir}/${PN}"

# we want htdocs and cgi-bin to go with the binary
FILES_${PN} += "${datadir}/${PN}/htdocs ${datadir}/${PN}/cgi-bin"

#make sure the lone .so links also get wrapped in the base package
FILES_${PN} += " ${libdir}/lib*.so ${libdir}/pkgconfig/*"

CFLAGS_append = " -DPATH_MAX=4096"
CFLAGS_prepend = "-I${STAGING_INCDIR}/openssl "
EXTRA_OECONF = "--enable-ssl \
		--with-ssl=${STAGING_LIBDIR}/.. \
		--enable-v4-mapped \
		--enable-logio \
		--disable-shared \
		--with-expat=${STAGING_LIBDIR}/.. \
		--enable-info \
		--enable-rewrite \
		--with-dbm=sdbm \
		--with-berkeley-db=no \
		--localstatedir=/var/${PN} \
		--with-gdbm=no \
		--with-ndbm=no \
		--includedir=${includedir}/${PN} \
		--datadir=${datadir}/${PN} \
		--sysconfdir=${sysconfdir}/${PN} \
		"

#
# here we over-ride the autotools provided do_configure.
#
do_configure() {
	oe_runconf
}

do_stage() {
	mkdir -p ${STAGING_INCDIR}/apache2
	mkdir -p ${STAGING_INCDIR}/apache2/include

	# TODO: just put this in apache2 not apache2/include
	cp -a include/*.h ${STAGING_INCDIR}/apache2/include
	cp -a srclib/apr/include/*.h ${STAGING_INCDIR}/apache2/include
	cp -a srclib/apr-util/include/*.h ${STAGING_INCDIR}/apache2/include
	cp -a os/unix/*.h ${STAGING_INCDIR}/apache2/include
}

do_install_append() {
	install -d ${D}/${sysconfdir}/init.d
	cat ${FILESDIR}/../files/init | \
		sed -e 's,/usr/sbin/,${sbindir}/,g' \
		    -e 's,/usr/bin/,${bindir}/,g' \
		    -e 's,/usr/lib,${libdir}/,g' \
		    -e 's,/etc/,${sysconfdir}/,g' \
		    -e 's,/usr/,${prefix}/,g' > ${D}/${sysconfdir}/init.d/${PN}
	chmod 755 ${D}/${sysconfdir}/init.d/${PN}
# remove the goofy original files...
	rm -rf ${D}/${sysconfdir}/${PN}/original
# Expat should be found in the staging area via DEPENDS...
	rm -f ${D}/${libdir}/libexpat.*
	cp -a ${FILESDIR}/httpd.conf ${D}/${sysconfdir}/${PN}
	cp -a ${FILESDIR}/httpd-ssl.conf ${D}/${sysconfdir}/${PN}/extra
	mkdir -p ${D}/${sysconfdir}/${PN}/conf.d
	ln -s /dev/null ${D}${base_prefix}/var/apache2/logs/error_log
}
