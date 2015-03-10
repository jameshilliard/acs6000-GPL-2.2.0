DESCRIPTION = "NIS client tools"
AUTHOR = "Thorsten Kukuk <kukuk@suse.de>"
HOMEPAGE = "http://www.linux-nis.org"
SECTION = "network"
PRIORITY = "optional"
LICENSE = "GPLv2"
DEPENDS = ""
PR = "r1"

# upstream does not yet publish any release so we have to fetch last working version from CVS
SRC_URI = "http://ftp.ntu.edu.tw/linux/utils/net/NIS/yp-tools-2.10.tar.gz"

inherit autotools
