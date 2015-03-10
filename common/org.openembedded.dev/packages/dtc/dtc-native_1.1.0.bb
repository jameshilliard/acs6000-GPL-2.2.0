DESCRIPTION = "Tool to compile a flattened device tree."
HOMEPAGE = "http://jdl.com/"
SECTION = "devel"
MAINTAINER = "Jon Loeliger <www.jdl.com>"
LICENSE = "GPL"

SRC_URI = "http://jdl.com/software/dtc-${PV}.tar.gz \
          "

S = "${WORKDIR}/dtc"

INHIBIT_AUTOTOOLS_DEPS = "1"
inherit autotools native

do_stage () {
	oe_runmake DESTDIR=${base_prefix} PREFIX= install
}
