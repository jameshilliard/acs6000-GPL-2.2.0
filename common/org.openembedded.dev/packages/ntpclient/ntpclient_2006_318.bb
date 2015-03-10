DESCRIPTION = "NTP (RFC-1305) client for unix-alike computers"
HOMEPAGE = "http://doolittle.icarus.com/ntpclient"
AUTHOR = "Larry Doolittle <larry@doolittle.boa.org>"
RDEPENDS = "busybox"
SECTION = "admin"
LICENSE = "GPLv2"
PR = "r1"
# The ntpclient package uses version numbers that include an underscore :(
PV = "2006_318"
# ntpclient unpacks into a directory that doesn't include complete version info :(
S = "${WORKDIR}/${PN}-2006"

SRC_URI = "\
		http://doolittle.icarus.com/ntpclient/ntpclient_${PV}.tar.gz \
		file://Makefile.patch;patch=1 \
		file://gettimeofday.patch;patch=1 \
		file://ntpclient-ipv6.patch;patch=1 \
		file://count.patch;patch=1 \
		file://init \
		file://default \
	"

#INITSCRIPT_NAME = "ntpclient"
#INITSCRIPT_PARAMS = "defaults 65"
#inherit update-rc.d

do_compile() {
    oe_runmake ntpclient
    oe_runmake adjtimex
}

do_install () {
    # Install the binary and tools
    install -D -m 0755 ${S}/ntpclient ${D}${base_sbindir}/ntpclient
    install -D -m 0755 ${S}/adjtimex ${D}${base_sbindir}/adjtimex
    install -D -m 0755 ${S}/rate.awk ${D}${sbindir}/ntpclient-drift-rate.awk
    install -D -m 0755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/ntpclient
    install -D -m 0755 ${WORKDIR}/default ${D}${sysconfdir}/default/ntpclient
}

