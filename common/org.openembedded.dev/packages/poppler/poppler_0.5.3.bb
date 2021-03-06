DESCRIPTION = "Poppler is a PDF rendering library based on the xpdf-3.0 code base."
MAINTAINER = "Koen Kooi <koen@dominion.kabel.utwente.nl>"
DEPENDS = "fontconfig jpeg gtk+ cairo"
LICENSE = "GPL"
PR = "r1"

SRC_URI = "http://poppler.freedesktop.org/${PN}-${PV}.tar.gz"

inherit autotools pkgconfig

EXTRA_OECONF = "--enable-xpdf-headers  --disable-gtk-test --disable-poppler-qt"  

do_stage() {
	autotools_stage_all
}
