DESCRIPTION="Timezone data (/usr/share/zoneinfo) and utilities (tzselect/zic/zdump)"
HOMEPAGE="ftp://elsie.nci.nih.gov/pub/"
LICENSE="GPL-2"
SECTION="sys-libs"

SRC_URI="ftp://elsie.nci.nih.gov/pub/tzdata${PV}.tar.gz \
    ftp://elsie.nci.nih.gov/pub/tzcode${PV}.tar.gz \
	file://timezone-data-2005n-makefile.patch;patch=1;pnum=0"

DEPENDS="timezone-data-native"

S=${WORKDIR}

FILES_${PN} += "/usr/share/zoneinfo"

do_install () {
	oe_runmake install DESTDIR=${D} zic="zic"
}
