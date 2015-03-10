DESCRIPTION="A command to eject a disc from the CD-ROM drive"
HOMEPAGE="http://eject.sourceforge.net/"

LICENSE="GPLv2"

SRC_URI="http://www.pobox.com/~tranter/${P}.tar.gz \
	file://${PN}-2.0.13-xmalloc.patch;patch=1;pnum=0 \
	file://${PN}-2.1.4-scsi-rdwr.patch;patch=1 \
	file://${PN}-2.1.5-handle-spaces.patch;patch=1;pnum=0 \
	"

S=${WORKDIR}/${PN}

inherit autotools

