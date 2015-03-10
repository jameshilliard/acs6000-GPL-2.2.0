DESCRIPTION = "\
PAM authentication library for Linux.  \
Linux-PAM (Pluggable Authentication Modules for Linux) is a \
library that enables the local system administrator to choose \
how individual applications authenticate users. For an \
overview of the Linux-PAM library see the Linux-PAM System \
Administrators' Guide."
HOMEPAGE = "http://kernel.org/pub/linux/libs/pam"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = "GPLv2"

DEPENDS="flex flex-native cracklib"
RDEPENDS="cracklib"

PR = "r2"

# The project is actually called Linux-PAM but that gives
# a bad OE package name because of the upper case characters
pn = "Linux-PAM"
p = "${pn}-${PV}"
S = "${WORKDIR}/${p}"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/libs/pam/pre/library/${p}.tar.bz2 \
           file://pam-nodocs.patch;patch=1 "

# GSP AAA support
SRC_URI += " file://libpam-gsp_aaa.patch;patch=1"
# Multibyte Support
SRC_URI += " file://libpam-cracklib_multibyte.patch;patch=1 \
             file://pam-modules.patch;patch=1 \
	     file://ja_po.patch;patch=1 "

#libdir=/usr/lib

inherit autotools

# EXTRA_OECONF += " --enable-static-libpam"
# Disable building of the documentation - it requires too many different
# programs installed on the build system and is a waste of time.  This
# leaves the man documentation in the build.
#EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2TXT=no"
#EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2HTML=no"
#EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2LATEX=no"
#EXTRA_OECONF += "ac_cv_prog_HAVE_PS2PDF=no"
#EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2PS=no"

LEAD_SONAME = "libpam.so.*"

# This is crude - the modules maybe should each have independent ipks
FILES_${PN} += "/lib/security/pam_*.so /lib/security/pam_filter/* /usr/lib/libpam*.* /usr/sbin/* /etc/* /usr/share/locale/ja_JP* /usr/share/locale/zh_CN* "
FILES_${PN}-dbg += "${libdir}/security/.debug"

# maintain the pam default layout
EXTRA_OECONF += " --includedir=${includedir}/security \
 		--enable-securedir=/lib/security "
	
do_compile_powerpc () {
 oe_runmake CRACKLIB_DICTS=/usr/lib/cracklib_dict
}

#do_configure() {
#	oe_runconf
#}

#do_stage() {
#	autotools_stage_all
#}

do_stage() {
    autotools_stage_includes
    for lib in libpam libpamc libpam_misc
    do
        oe_libinstall -so -C "$lib" "$lib" ${STAGING_LIBDIR}
    done
}
 
do_install_append_acs6000() {
	pushd ${S}/po
	make update-gmo
	popd
        install -d ${D}/usr/share
        install -d ${D}/usr/share/locale
        install -d ${D}/usr/share/locale/ja_JP.UTF-8
        install -d ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES
        install -m 0644 ${S}/po/ja.gmo ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES/Linux-PAM.mo
        install -d ${D}/usr/share/locale/zh_CN.UTF-8
        install -d ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES
        install -m 0644 ${S}/po/zh_CN.gmo ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES/Linux-PAM.mo
}

