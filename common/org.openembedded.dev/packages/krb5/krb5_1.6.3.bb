DESCRIPTION = "krb5 MIT v1.6.3 package"
HOMEPAGE = "http://web.mit.edu/Kerberos/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "libs"
PRIORITY = "optional"
LICENSE = "unknown"
DEPENDS = "ncurses"
PR = "r1"

SRC_URI = "http://web.mit.edu/kerberos/dist/krb5/1.6/krb5-1.6.3-signed.tar \
        file://crosscompiling.patch;patch=1 \
        file://gsp_aaa.patch;patch=1 \
	file://krb5-config.in "

S=${WORKDIR}/krb5-${PV}/src

inherit autotools

#CFLAGS_append = " -O0 -g -ggdb "

krb5_unpack () {
    $(cd ${WORKDIR}; tar -xzf ${P}.tar.gz; rm -f ${P}.tar.gz)
}

python do_unpack_append () {
    bb.build.exec_func ('krb5_unpack', d)
}

do_configure () {
    install -m 0644 ${WORKDIR}/krb5-config.in ${S}/
    oe_runconf "cross_compiling=yes \
       krb5_cv_attr_constructor_destructor=yes,yes \
        krb5_cv_func_swab_noproto=no \
        ac_cv_func_regcomp=yes \
        ac_cv_file__etc_environment=yes \
        ac_cv_file__etc_TIMEZONE=yes "
}

do_stage() {
    cp -a ${WORKDIR}/install/krb5-dev/${includedir}/* ${STAGING_INCDIR}

    cp -a ${WORKDIR}/install/krb5/usr/lib/lib*.so* ${STAGING_LIBDIR}
    ln -sf libkrb5support.so.0.1 ${STAGING_LIBDIR}/libkrb5support.so
    ln -sf libkrb5.so.3.3 ${STAGING_LIBDIR}/libkrb5.so
    ln -sf libkrb4.so.2.0  ${STAGING_LIBDIR}/libkrb4.so
    ln -sf libkdb5.so.4.0 ${STAGING_LIBDIR}/libkdb5.so
    ln -sf libk5crypto.so.3.1 ${STAGING_LIBDIR}/libk5crypto.so
    ln -sf libgssapi_krb5.so.2.2 ${STAGING_LIBDIR}/libgssapi_krb5.so
    ln -sf libcom_err.so.3.0 ${STAGING_LIBDIR}/libcom_err.so

    install -d ${STAGING_LIBDIR}/bin
    cp -a ${WORKDIR}/install/krb5/usr/lib/krb5 ${STAGING_LIBDIR}
    cp -a ${WORKDIR}/install/krb5/usr/bin/krb5-config ${STAGING_LIBDIR}/bin
}
