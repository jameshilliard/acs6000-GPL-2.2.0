DESCRIPTION = "Password Checking Library"
AUTHOR = "Nathan Neulinger <nneul@umr.edu> Alec Muffett <alecm@crypto.dircon.co.uk>"
HOMEPAGE = "http://sourceforge.net/projects/cracklib"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
SECTION = "lib"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

SRC_URI = "${SOURCEFORGE_MIRROR}/cracklib/cracklib-${PV}.tar.gz"

S = ${WORKDIR}/cracklib-${PV}

inherit autotools native
