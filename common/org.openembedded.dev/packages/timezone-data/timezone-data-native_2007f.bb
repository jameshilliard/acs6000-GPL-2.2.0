require timezone-data_${PV}.bb

inherit native

DEPENDS = ""

do_stage () {
	install zic ${STAGING_BINDIR}
}
