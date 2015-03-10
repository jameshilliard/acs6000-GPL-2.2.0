# This package builds the NIS ypbind daemon
# The source package is utils/net/NIS/ypbind-mt
#
PR = "r0"
DESCRIPTION="\
Multithreaded NIS bind service (ypbind-mt).  \
ypbind-mt is a complete new implementation of a NIS \
binding daemon for Linux. It has the following \
features.  Supports ypbind protocol V1 and V2.  \
Uses threads for better response.  Supports multiple \
domain bindings.  Supports /var/yp/binding/* file \
for Linux libc 4/5 and glibc 2.x.  Supports a list \
of known secure NIS server (/etc/yp.conf) Binds to \
the server which answered as first."
HOMEPAGE="http://www.linux-nis.org/nis/ypbind-mt/index.html"

require nis.inc

SRC_URI += " file://ypbind.init"

# ypbind-mt now provides all the functionality of ypbind
# and is used in place of it.
PROVIDES += "ypbind"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -D -m 644 ${S}/etc/yp.conf ${D}/etc/yp.conf
	install -D -m 755 ${WORKDIR}/ypbind.init ${D}/etc/init.d/ypbind
}

# prepended 'away_' to disable starting ypbind by default:
away_pkg_postinst_ypbind-mt () {
  if test "x$D" != "x"; then
	exit 1
  else
    update-rc.d ypbind defaults 90
    exit 0
  fi
}

away_pkg_postrm_ypbind-mt () {
  if test "x$D" != "x"; then
	exit 1
  else
    ${sysconfdir}/init.d/ypbind stop
    update-rc.d -f ypbind remove
  fi
}

