DESCRIPTION="GNU CGI Library in C"
SECTION="libs"
Maintainer = "Jiff Shen <jiff.shen@avocent.com>"
Homepage= "http://catchen.org/gcgi/"
LICENCE = "LGPL"

SRC_URI = "http://catchen.org/gcgi/${P}.tar.gz"

S = ${WORKDIR}/libgcgi.a-${PV}

EXTRA_OECONF = "--with-openssl"

inherit autotools

