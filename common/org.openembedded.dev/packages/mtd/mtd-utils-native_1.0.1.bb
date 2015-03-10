LICENSE = "GPLv2"
SECTION = "base"
require mtd-utils_${PV}.bb
inherit native
DEPENDS = "zlib-native"
FILESDIR = "${@os.path.dirname(bb.data.getVar('FILE',d,1))}/mtd-utils"

do_stage () {
	oe_runmake install DESTDIR=${STAGING_DIR}
}
