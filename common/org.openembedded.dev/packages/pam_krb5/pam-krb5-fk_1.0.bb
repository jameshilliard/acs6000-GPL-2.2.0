DESCRIPTION = "PAM module which support Kerberos 5 authentication"
AUTHOR = "Frank Cusack <fcusack@fcusack.com>"
SECTION = "libs"
LICENSE = "GPLv1"
DEPENDS = "libpam krb5"
PR = "r1"

DEPENDS="libpam krb5"
RDEPENDS="libpam krb5"

SRC_URI = "file://pam_krb5_fk-1.0.tar.gz \
    file://errno.patch;patch=1 \
    file://gsp_aaa.patch;patch=1 \
    "
S = "${WORKDIR}/pam_krb5_fk-${PV}"

LDFLAGS += "-shared"
EXTRA_OEMAKE = '-e OSLIBS="-lnsl -lresolv -lpam -lcom_err" KRB5LIBS="-lkrb5 -lk5crypto -lkrb5support" INC="" '

do_install () {
    install -m 0755 -d ${D}/lib/security
    install -m 0755 pam_krb5.so.1 ${D}/lib/security
    ln -sf pam_krb5.so.1 ${D}/lib/security/pam_krb5.so
    install -m 0755 -d ${D}/${sysconfdir}
    install -m 0644 ${FILESDIR}/krb5.conf ${D}/${sysconfdir}
    install -m 0644 ${FILESDIR}/krb5.keytab ${D}/${sysconfdir}
}

FILES_${PN}-dbg += "/lib/security/.debug"
FILES_${PN} += "/lib/security ${sysconfdir}"
