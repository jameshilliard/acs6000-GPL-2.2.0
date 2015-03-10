# PAM authentication library for Linux - Linux-PAM
#  
# NOTE: this is a library with plug-in modules, at present all
# the modules are built and installed into the main libpam
# ipkg.  This causes lots of problems (e.g. it is not possible
# to build on uClibC) so *do not* rely on this behaviour -
# assume the modules will be moved to individual ipks (like
# the kernel modules.)
#
DESCRIPTION = "\
PAM authentication library for Linux.  \
Linux-PAM (Pluggable Authentication Modules for Linux) is a \
library that enables the local system administrator to choose \
how individual applications authenticate users. For an \
overview of the Linux-PAM library see the Linux-PAM System \
Administrators' Guide."
HOMEPAGE = "http://www.kernel.org/pub/linux/libs/pam"
MAINTAINER = "John Bowler <jbowler@acm.org>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = "GPL-2"
PR = "r1"

PARALLEL_MAKE = ""

# The project is actually called Linux-PAM but that gives
# a bad OE package name because of the upper case characters
pn = "Linux-PAM"
p = "${pn}-${PV}"
S = "${WORKDIR}/${p}"

SRC_URI = "http://www.kernel.org/pub/linux/libs/pam/pre/library/${p}.tar.gz;localpath=${DL_DIR}/${p}.tar.gz"

inherit autotools

# EXTRA_OECONF += " --enable-static-libpam"
# Disable building of the documentation - it requires too many different
# programs installed on the build system and is a waste of time.  This
# leaves the man documentation in the build.

EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2TXT=no"
EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2HTML=no"
EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2LATEX=no"
EXTRA_OECONF += "ac_cv_prog_HAVE_PS2PDF=no"
EXTRA_OECONF += "ac_cv_prog_HAVE_SGML2PS=no"
EXTRA_OECONF += "cross_compiling=yes FAKEROOT=${D} --exec_prefix=${D} --prefix=${D}"

LEAD_SONAME = "libpam.so.*"

# This is crude - the modules maybe should each have independent ipks
#FILES_${PN} += "/usr/lib/security/pam_*.so /usr/lib/security/pam_filter/*"
FILES_${PN} += "/usr/lib/* /usr/sbin/* /etc/*"
FILES_${PN}-dbg += "${libdir}/security/.debug"

do_configure() {
    oe_runconf 
}

do_install() {
    oe_runmake install
}

do_stage() {
    install -d ${STAGING_INCDIR}/security
    install -d ${STAGING_LIBDIR}/security
    for header in ${S}/libpam/include/security/* ${S}/libpamc/include/security/* ${S}/libpam_misc/include/security/*
    do
        install -m 0755 ${header} ${STAGING_INCDIR}/security
    done
    for lib in ${S}/libpam/*.so ${S}/libpamc/*.so ${S}/libpam_misc/*.so
    do
        install -m 0644 ${lib} ${STAGING_LIBDIR}
    done
    for lib in ${S}/modules/*/*.so
    do
        install -m 0644 ${lib} ${STAGING_LIBDIR}/security
    done
}

# An attempt to build on uclibc will fail, causing annoyance,
# so force the package to be skipped here (this will cause a
# 'nothing provides' error)
#NOTE: this can be fixed, but it means hacking the modules so
# that those which use YP don't get built on uClibC, this looks
# like a big patch...
python () {
    os = bb.data.getVar("TARGET_OS", d, 1)
    if os == "linux-uclibc":
        raise bb.parse.SkipPackage("Some PAM modules require rpcsvc/yp.h, uClibC does not provide this")
}
