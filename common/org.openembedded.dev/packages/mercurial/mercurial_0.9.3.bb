DESCRIPTION =  "scalable distributed SCM"
HOMEPAGE    =  "http://www.selenic.com/mercurial/"
SECTION     =  "console/network"
PRIORITY    =  "optional"
LICENSE     =  "GPL-2"
MAINTAINER  =  "Lynn Lin <lynn.lin@avocent.com>"
PR          =  "r0"


SRC_URI     =  "http://www.selenic.com/mercurial/release/${PN}-${PV}.tar.gz"

inherit distutils

do_compile() {
	:
}
