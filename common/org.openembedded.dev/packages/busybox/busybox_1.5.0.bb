require busybox.inc

PR = "r2"

DEFAULT_PREFERENCE = "-1"

DEPENDS = "gsp-auth libpam dlog libcyevent typesafe-api"

SRC_URI += "file://dhcp-increase-retries.patch;patch=1 \
            file://df_rootfs.patch;patch=1 \
            file://ipv6.patch;patch=1 \
            file://ld-powerpc.patch;patch=1 \
            file://coreutils-dd-swab.patch;patch=1 \
            file://include-usage.patch;patch=1 \
            file://networking-ping.patch;patch=1 \
            file://networking-route-resolve_from_host.patch;patch=1 \
            file://init-halt-cynotify.patch;patch=1 \
            file://procps-top-sys.patch;patch=1 \
            file://start-stop-daemon-deleted-files.patch;patch=1 \
            file://login_nohostname.patch;patch=1 \
            file://defconfig \
	    file://udhcpc.fail"

# Added additional Avocent baud rates to speed_table.c
SRC_URI_append_amazon = " file://baudrates.patch;patch=1 "

SRC_URI_append_GSP = " file://GSP-getty.patch;patch=1"

SRC_URI_append = " file://gsp_aaa.patch;patch=1"

SRC_URI_append_pmhd = " file://pmcli_login.patch;patch=1"

SRC_URI_append_acs6000 = " file://start-stop-locale.patch;patch=1 \
			   file://login_utf8.patch;patch=1 \
			   file://telnet_none.patch;patch=1 \
			"

export LDLIBS="-lgsp_auth -lcyc -lcyc++ -lpam -lpam_misc -ldl -lstdc++ -lcyevent -lgsp_tsapi -lglib-2.0 -lcacpdparser -ldaemon -ldlog -L${STAGING_LIBDIR}"

EXTRA_OEMAKE_append = " V=1 ARCH=${TARGET_ARCH} CROSS_COMPILE=${TARGET_PREFIX}"
#CFLAGS_append = " -O0 -g -ggdb -I${STAGING_INCDIR}/glib-2.0 -DFEATURE_SWAB -DALTERPATH -DCYTIME -DMASK_FROM_TARGET -DCYNOTIFY -DUSE_FEATURE_TOP_CPU_USAGE_PERCENTAGE"
CFLAGS_append = " -I${STAGING_INCDIR}/glib-2.0 -DFEATURE_SWAB -DALTERPATH -DCYTIME -DMASK_FROM_TARGET -DCYNOTIFY -DUSE_FEATURE_TOP_CPU_USAGE_PERCENTAGE"

do_configure () {
	install -m 0644 ${WORKDIR}/defconfig ${S}/.config
	cml1_do_configure
}

do_compile () {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	base_do_compile
}

do_install () {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	install -d ${D}${sysconfdir}/init.d
	oe_runmake "PREFIX=${D}" install
	cp -pPR ${S}/_install/* ${D}/

	# Move everything to /busybox (not supposed to end up in any package)
	install -d ${D}/busybox
	ls ${D} -R

	cp -dPr ${D}${base_bindir} ${D}${base_sbindir} ${D}${prefix} ${D}/busybox/
	# Move the busybox binary back to /bin
	install -d ${D}${base_bindir}
	mv ${D}/busybox${base_bindir}/busybox ${D}${base_bindir}/
	# Move back the sh symlink
	test -h ${D}/busybox${base_bindir}/sh && mv ${D}/busybox${base_bindir}/sh ${D}${base_bindir}/

	if grep "CONFIG_SYSLOGD=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/syslog ${D}${sysconfdir}/init.d/
		install -m 644 ${WORKDIR}/syslog.conf ${D}${sysconfdir}/
	fi
	if grep "CONFIG_CROND=y" ${WORKDIR}/defconfig; then
		# Move crond back to /usr/sbin/crond
		install -d ${D}${sbindir}
		mv ${D}/busybox${sbindir}/crond ${D}${sbindir}/

		install -m 0755 ${WORKDIR}/busybox-cron ${D}${sysconfdir}/init.d/
	fi
	if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
		# Move httpd back to /usr/sbin/httpd
		install -d ${D}${sbindir}
		mv ${D}/busybox${sbindir}/httpd ${D}${sbindir}/

		install -m 0755 ${WORKDIR}/busybox-httpd ${D}${sysconfdir}/init.d/
		install -d ${D}/srv/www
	fi
	if grep "CONFIG_APP_UDHCPD=y" ${WORKDIR}/defconfig; then
		# Move udhcpd back to /usr/sbin/udhcpd
		install -d ${D}${sbindir}
		mv ${D}/busybox${sbindir}/udhcpd ${D}${sbindir}/

		install -m 0755 ${WORKDIR}/busybox-udhcpd ${D}${sysconfdir}/init.d/
	fi
	if grep "CONFIG_HWCLOCK=y" ${WORKDIR}/defconfig; then
		# Move hwclock back to /sbin/hwclock
		install -d ${D}${base_sbindir}
		mv ${D}/busybox${base_sbindir}/hwclock ${D}${base_sbindir}/

		install -m 0755 ${WORKDIR}/hwclock.sh ${D}${sysconfdir}/init.d/
	fi
	if grep "CONFIG_APP_UDHCPC=y" ${WORKDIR}/defconfig; then
		# Move dhcpc back to /usr/sbin/udhcpc
		install -d ${D}${base_sbindir}
		mv ${D}/busybox${base_sbindir}/udhcpc ${D}${base_sbindir}/

		install -d ${D}${sysconfdir}/udhcpc.d
		install -d ${D}${datadir}/udhcpc
		install -m 0755 ${S}/examples/udhcp/simple.script ${D}${sysconfdir}/udhcpc.d/50default
		install -m 0755 ${WORKDIR}/default.script ${D}${datadir}/udhcpc/default.script
	fi

	install -m 0644 ${S}/busybox.links ${D}${sysconfdir}
}

do_install_append_GSP () {
	if grep "CONFIG_APP_UDHCPC=y" ${WORKDIR}/defconfig; then
		install ${WORKDIR}/udhcpc.fail ${D}${sysconfdir}/udhcpc.d/50fail
	fi
}

pkg_prerm_${PN} () {
	# This is so you can make busybox commit suicide - removing busybox with no other packages
	# providing its files, this will make update-alternatives work, but the update-rc.d part
	# for syslog, httpd and/or udhcpd will fail if there is no other package providing sh
	tmpdir=`mktemp -d /tmp/busyboxrm-XXXXXX`
	ln -s /bin/busybox $tmpdir/[
	ln -s /bin/busybox $tmpdir/test
	ln -s /bin/busybox $tmpdir/head
	ln -s /bin/busybox $tmpdir/sh
	ln -s /bin/busybox $tmpdir/basename
	ln -s /bin/busybox $tmpdir/echo
	ln -s /bin/busybox $tmpdir/mv
	ln -s /bin/busybox $tmpdir/ln
	ln -s /bin/busybox $tmpdir/dirname
	ln -s /bin/busybox $tmpdir/rm
	ln -s /bin/busybox $tmpdir/sed
	ln -s /bin/busybox $tmpdir/sort
	export PATH=$PATH:$tmpdir
	while read link; do case "$link" in /*/*/*) to="../../bin/busybox";; /bin/*) to="busybox";; /*/*) to="../bin/busybox";; esac; bn=`basename $link`; sh /usr/bin/update-alternatives --remove $bn $to; done </etc/busybox.links
}
