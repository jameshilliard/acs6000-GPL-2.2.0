DESCRIPTION = "Various tools relating to the Simple Network Management Protocol"
HOMEPAGE = "http://www.net-snmp.org/"
LICENSE = "BSD"
MAINTAINER = "Bruno Randolf <bruno.randolf@4g-systems.biz>"
DEPENDS = "openssl"
PR = "r1"

SRC_URI = "${SOURCEFORGE_MIRROR}/net-snmp/net-snmp-${PV}.tar.gz \
	file://bug52655.patch;patch=1 \
	file://uclibc-fix.patch;patch=1 \
	file://init \
	file://snmpd.conf \
	file://snmptrapd.conf "
	
SRC_URI_append_GSP = "file://net-snmp-CYC.patch;patch=1 \
			  file://init.GSP "

SRC_URI_append_acs6000 = "file://net-snmp-CYC_acs6000.patch;patch=1 \
			  file://snmpd.conf.acs6000 \
			  file://snmpd.local.conf.acs6000 "

SRC_URI_append_pmhd = "file://net-snmp-CYC_pmhd.patch;patch=1 \
			  file://snmpd.conf.pmhd \
			  file://snmpd.local.conf.pmhd "

inherit autotools

PARALLEL_MAKE = ""
EXTRA_OECONF = "--enable-shared --disable-manuals" 
EXTRA_OEMAKE = "INSTALL_PREFIX=${D}"

do_configure() {
	# endianness fun... inspired by openssl.inc
	. ${CONFIG_SITE}
	if [ "x$ac_cv_c_bigendian" = "xyes" -o "x$ac_cv_c_littleendian" = "xno" ]; then
	    ENDIANESS=" --with-endianness=big"
	elif [ "x$ac_cv_c_littleendian" = "xyes" -o "x$ac_cv_c_bigendian" = "xno" ]; then
	    ENDIANESS=" --with-endianness=little"
	else
	    oefatal do_configure cannot determine endianess
	fi
	oenote Determined endianess as: $ENDIANESS
	oe_runconf $ENDIANESS
}

EXTRAGSP=" \
          --disable-scripts \
          --disable-mibs \
          --disable-mib-loading \
	  --enable-shared \
	  --enable-ipv6 \
          --with-transports="UDP TCP UDPIPv6" \
          --with-out-transports="Callback Unix TCPIPv6 IPX ALL5PVC" \
          --with-mib-modules="mibII,ucd_snmp,utilities,snmpv3mibs,if-mib" \
          --with-out-mib-modules="notification,target,agentx,host,agent_mibs,ucd-snmp/versioninfo" \
	"

EXTRAACS6000=" \
          --with-enterprise-oid=10418 \
          --with-enterprise-sysoid=.1.3.6.1.4.1.10418.16.2 \
          --with-default-snmp-version="3" \
          --with-sys-contact="administrator" \
          --with-sys-location="Cyclades_ACS_6000" \
          --with-endianness=big \
          ${EXTRAGSP} \
	"

EXTRAPMHD=" \
          --with-enterprise-oid=10418 \
          --with-enterprise-sysoid=.1.3.6.1.4.1.10418.17.2 \
          --with-default-snmp-version="3" \
          --with-sys-contact="administrator" \
          --with-sys-location="Avocent_PM" \
          --with-endianness=big \
          ${EXTRAGSP} \
	"


do_configure_acs6000() {
	. ${CONFIG_SITE}
	oe_runconf ${EXTRAACS6000}
}
do_configure_pmhd() {
	. ${CONFIG_SITE}
	oe_runconf ${EXTRAPMHD}
}

do_stage_GSP(){
        cp --dereference -Rv include/* ${STAGING_INCDIR}/
}

do_install_append() {
	install -d ${D}${sysconfdir}/snmp
	install -d ${D}${sysconfdir}/init.d
	if [ "${DISTRO}" = "acs6000" ]; then
		install -m 755 ${WORKDIR}/init.GSP ${D}${sysconfdir}/init.d/snmpd
		install -m 644 ${WORKDIR}/snmpd.conf.acs6000 ${D}${sysconfdir}/snmp/snmpd.conf
		install -m 644 ${WORKDIR}/snmpd.local.conf.acs6000 ${D}${sysconfdir}/snmp/snmpd.local.conf

	else if [ "${DISTRO}" = "pmhd" ]; then
		install -m 755 ${WORKDIR}/init.GSP ${D}${sysconfdir}/init.d/snmpd
		install -m 644 ${WORKDIR}/snmpd.conf.pmhd ${D}${sysconfdir}/snmp/snmpd.conf
		install -m 644 ${WORKDIR}/snmpd.local.conf.pmhd ${D}${sysconfdir}/snmp/snmpd.local.conf

		else
			install -m 755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/snmpd
			install -m 644 ${WORKDIR}/snmptrapd.conf ${D}${sysconfdir}/snmp/
			install -m 644 ${WORKDIR}/snmpd.conf ${D}${sysconfdir}/snmp/
		fi
	fi
}

PACKAGES = "net-snmp-dbg net-snmp-doc net-snmp-dev net-snmp-libs net-snmp-mibs net-snmp-server net-snmp-client"
FILES_net-snmp-libs = "${libdir}/*"
FILES_net-snmp-mibs = "${datadir}/snmp/mibs"
FILES_net-snmp-server = "${sbindir}/* ${sysconfdir}"
FILES_net-snmp-client = "${bindir}/* ${datadir}/snmp/"
FILES_net-snmp-dbg += "${libdir}/.debug/ ${sbindir}/.debug/ ${bindir}/.debug/"

#following Jiff suggestion
#If  net-snmp-mibs is not necessary,  just comment out these lines. I think other projects do not need it either
#RDEPENDS_net-snmp-server += "net-snmp-mibs"
#RDEPENDS_net-snmp-client += "net-snmp-mibs"

pkg_postinst_net-snmp-server() {
if test "x$D" != "x"; then
	D="-r $D"
else
	D="-s"
fi
update-rc.d $D snmpd defaults
}

pkg_postrm_net-snmp-server() {
if test "x$D" != "x"; then
	D="-r $D"
else
	/etc/init.d/snmpd stop
fi
update-rc.d $D snmpd remove
}

CONFFILES_${PN}_nylon = "${sysconfdir}/snmp/snmpd.conf ${sysconfdir}/snmp/snmptrapd.conf"
