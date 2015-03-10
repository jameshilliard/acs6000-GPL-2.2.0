DESCRIPTION = "event notification."

MODULE_NAME = "event-notify-headers"

DEPENDS = ""
RDEPENDS = ""

INSTALL_FILES  = " event_notif.conf:/etc:0644 "

EXPORT_H =event_connect.h event_def.h event_conf.h event_msgs.h trap_oids.h

inherit avocentgsp

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};rev=209;localpath=${DL_DIR}/${MODULE_NAME} \
    file://mib-events.conf"

SRC_URI_append_acs6k-oem1 = " \
	file://GSP-TRAP-MIB.temp;patch=1 \
	"

S=${WORKDIR}/${MODULE_NAME}

PACKAGES = "${PN}"

FILES_${PN} = "${sysconfdir}/* usr/local/mibs/*.asn /usr/share/event_properties/*"

do_configure_append() {
        cp ${WORKDIR}/mib-events.conf ${S}/mib-events.conf
}

do_compile() {
	unix2dos GSP-TRAP-MIB.temp
	perl template2mib.pl GSP-TRAP-MIB.temp mib-events.conf
        cp *.asn TRAP-MIB
        perl mib2traps.pl TRAP-MIB
        perl mib2event_h.pl TRAP-MIB mib-events.conf
	dos2unix *.asn
        rm TRAP-MIB
}

do_install_append () {
	install -d ${D}/${sysconfdir}
	install -m 0644 ${S}/event_shell ${D}/${sysconfdir}
	install -d ${D}/usr/local/mibs
        install -m 0444 *.asn ${D}/usr/local/mibs
	install -m 0444 *.asn ${D}
	install -d ${D}/usr/share/event_properties
        install -m 0644 events.properties ${D}/usr/share/event_properties/events.properties.en
}

do_stage_append() {
        install -d ${STAGING_DATADIR}/properties
        install -m 0444 ${S}/nmm_events.properties ${STAGING_DATADIR}/properties
}

