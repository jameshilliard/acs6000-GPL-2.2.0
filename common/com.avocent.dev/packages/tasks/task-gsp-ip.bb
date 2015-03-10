DESCRIPTION = "Task packages for GSP" 
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r1"

PACKAGES = "\
    task-gsp-apps-ip \
	"

RDEPENDS_task-gsp-apps-ip := "\
	logpipe \
	cacpd \
	cli3 \
	cli \
	dlog \
	pam-redirect \
	pam-otk \
	pam-shm \
	nss-shm \
	libcyc \
	gsp-session \
	avct-sendmail \
	event-notify \
	dataflow-msg \
	gsp-dbuf \
	libavctci \
	ci-interactive \
	libcacpdparser \
 	uicontrold \
	ap-module \
 	web \
	power-mgmt \
	cacpd-feval \
	gsp-air \
	avct-adsap2 \
	pam-adsap2 \
	avct-aidp \
	avct-aidp-fmnetwork \
	fm-aidp \
    fm-all \
    typesafe-api \
	gsp-power-api \
    "

RDEPENDS_task-gsp-apps-ip_append_powerpc := "\
	cmd-uboot \
	"

RDEPENDS_task-gsp-apps-ip_append_acs6000 := "\
	plugdev \
	"

RDEPENDS_task-gsp-apps-ip_append_alterpath := "\
        avct-adsap2-ext-sample \
        "

RDEPENDS_task-gsp-apps-ip_append_acs6000 := "\
	avct-adsap2-ext-acs \
	"

RDEPENDS_task-gsp-apps-ip_append_pmhd := "\
	avct-adsap2-ext-pmhd \
	"

