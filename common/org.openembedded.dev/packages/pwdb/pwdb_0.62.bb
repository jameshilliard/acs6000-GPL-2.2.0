SRC_URI = "${GENTOO_MIRROR}/${P}.tar.gz \
	file://install.patch;patch=1"

DEPENDS = ""

inherit autotools

do_configure_prepend () {
	sed -i -e "s/^DIRS = .*/DIRS = libpwdb/" \
		-e "s:EXTRAS += :EXTRAS += ${CFLAGS} :" \
		Makefile
	sed -i -e "s:=gcc:=${CC}:g" -e "s:=ld:=${LD}:g" -e "s:=ranlib:=${RANLIB}:g" default.defs 
	sed -i -e "s:PPFIX=.*:PPFIX=${D}:g" default.defs 
}

do_install_prepend () {
	install -d ${D}${base_libdir}
}

do_stage () {
	cp -a include/pwdb ${STAGING_INCDIR}
	( cd libpwdb;
		install -d ${STAGING_INCDIR}/pwdb;
    	install -m 644 pwdb/pwdb_public.h pwdb_map.h ${STAGING_INCDIR}/pwdb;
		install -m 644 unix/public.h ${STAGING_INCDIR}/pwdb/pwdb_unix.h;
	    install -m 644 shadow/public.h ${STAGING_INCDIR}/pwdb/pwdb_shadow.h;
		install -m 644 common/public.h ${STAGING_INCDIR}/pwdb/pwdb_common.h;
		install -m 644 radius/public.h ${STAGING_INCDIR}/pwdb/pwdb_radius.h;
		install -m 644 radius.h ${STAGING_INCDIR}/pwdb/radius.h;
		install -m 644 _pwdb_macros.h ${STAGING_INCDIR}/pwdb/_pwdb_macros.h;
	)

	oe_libinstall -s -C libpwdb libpwdb ${STAGING_LIBDIR}
}
