DESCRIPTION = "NTP daemon"
SECTION = "base"
PRIORITY = "optional"
HOMEPAGE = "http://www.ntp.org"

SRC_URI = " http://www.eecis.udel.edu/~ntp/ntp_spool/ntp4/ntp-4.2.0.tar.gz\
	   file://oe_build.patch;patch=1 \
	"
inherit autotools

EXTRA_OECONF = "--without-openssl-libdir"
