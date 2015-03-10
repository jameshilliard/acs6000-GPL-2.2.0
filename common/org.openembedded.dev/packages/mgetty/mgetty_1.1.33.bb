SECTION = "base"
DESCRIPTION = "The mgetty package contains an intelligent \
getty for allowing logins over a serial line (such as \
through a modem) and receiving incoming faxes."
LICENSE = "GPL"
PR="r3"

# The source can no longer be found at ${DEBIAN_MIRROR}/main/m/mgetty/mgetty_${PV}.orig.tar.gz
# so the nslu2-linux project has mirrored it until someone updates this package to a newer version.
SRC_URI = "ftp://mgetty.greenie.net/pub/mgetty/source/1.1/mgetty1.1.33-Apr10.tar.gz \
           file://mksed.patch;patch=1 \
           file://mksed"

SRC_URI_append_acs6000 = " file://acs6000_policy.patch;patch=1 \
			  file://acs6000-mgetty-config.patch;patch=1 \
			  file://login.cfg.in \
			  file://mgetty.cfg.in "

#CFLAGS_prepend = "-DAUTO_PPP -DFIDO "

# This is necessary because of the way the mgetty Makefile works,
# it effectively recursively makes . - without passing MAKE the -e
# flag.  Oops.
#export MAKE = "make -e"

do_compile () {
        cp ${S}/policy.h-dist ${S}/policy.h
	oe_runmake
}

do_compile_acs6000 () {
        cp ${S}/policy.h-dist ${S}/policy.h
	install -m 0600 ${WORKDIR}/login.cfg.in ${S}/ 
	oe_runmake
}

do_install () {
	install -d ${D}/var/spool
	oe_runmake install DESTDIR="${D}" prefix="${D}"
}

do_install_append_acs6000 (){
	install -d ${D}/bin
	install callback/callback ${D}/sbin/callback
}

pkg_postinst_mgetty_acs6000() {
        exit 0
}

pkg_postinst_mgetty() {
if test "x$D" = "x"; then
        addgroup -g 31 fax || true
        adduser --system --no-create-home --disabled-password --ingroup fax -s /bin/false fax
        chown fax /lib/mgetty+sendfax/faxq-helper
        chown fax /var/spool/fax
        chown fax /var/spool/fax/outgoing
else
        exit 1
fi
}

