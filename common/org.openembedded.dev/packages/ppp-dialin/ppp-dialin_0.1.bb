SECTION = "console/network"
DESCRIPTION = "Enables PPP dial-in through a serial connection"
MAINTAINER = "Rene Wagner <rw@handhelds.org>"
DEPENDS = "ppp"
RDEPENDS = "ppp"
PR = "r4"
LICENSE = "MIT"

SRC_URI = "file://host-peer \
           file://ppp-dialin"

SRC_URI_acs6000 = "file://cdma-peers \
                   file://cdma_chat-peers \
                   file://gsm-peers \
                   file://gsm_chat-peers \
                   "

do_install() {
	install -d ${D}${sysconfdir}/ppp/peers
	install -m 0644 ${WORKDIR}/host-peer ${D}${sysconfdir}/ppp/peers/host

	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/ppp-dialin ${D}${sbindir}
}

do_install_acs6000() {
	install -d ${D}${sysconfdir}/ppp/peers
	install -m 0640 ${WORKDIR}/cdma-peers ${D}${sysconfdir}/ppp/peers/cdma
	install -m 0640 ${WORKDIR}/cdma_chat-peers ${D}${sysconfdir}/ppp/peers/cdma_chat
	install -m 0640 ${WORKDIR}/gsm-peers ${D}${sysconfdir}/ppp/peers/gsm
	install -m 0640 ${WORKDIR}/gsm_chat-peers ${D}${sysconfdir}/ppp/peers/gsm_chat
}

pkg_postinst() {
if test "x$D" != "x"; then
	exit 1
else
	adduser --system --home /dev/null --no-create-home --empty-password --ingroup nogroup -s ${sbindir}/ppp-dialin ppp
fi
}

pkg_postrm() {
if test "x$D" != "x"; then
	exit 1
else
	deluser ppp
fi
}

pkg_postinst_acs6000() {
if test "x$D" != "x"; then
	exit 1
fi
}

pkg_postrm_acs6000() {
if test "x$D" != "x"; then
	exit 1
fi
}
