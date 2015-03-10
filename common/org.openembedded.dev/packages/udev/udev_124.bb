DESCRIPTION = "udev is a daemon which dynamically creates and removes device nodes from \
/dev/, handles hotplug events and loads drivers at boot time. It replaces \
the hotplug package and requires a kernel not older than 2.6.15."

LICENSE = "GPL"

PR = "r1"

SRC_URI = "http://kernel.org/pub/linux/utils/kernel/hotplug/udev-${PV}.tar.gz \
	   file://mount.blacklist \
	   file://create_udev_rule.sh \
	   file://udev_rules_awk \
	   file://udev.rules \
	   file://devfs-udev.rules \
	   file://links.conf \
           file://permissions.rules \
	   file://mount.sh \
	   file://network.sh \
	   file://call_plugdev.sh \
	   file://local.rules \
           file://init \
	"

SRC_URI_append_acs6000 = " file://serial.rules file://serial.sh "

UDEV_DEVFS_RULES ?= "0"

INITSCRIPT_NAME = "udev"
INITSCRIPT_PARAMS = "start 03 S ."

export CROSS = "${TARGET_PREFIX}"
export HOSTCC = "${BUILD_CC}"
export udevdir ?= "/dev"
export usrbindir := "${bindir}"
export usrsbindir := "${sbindir}"
export etcdir = "${sysconfdir}"
LD = "${CC}"
bindir = "/bin"
sbindir = "/sbin"

UDEV_EXTRAS = "extras/usb_id"
FILES_${PN} += "${usrbindir}/* ${usrsbindir}/*"
FILES_${PN}-dbg += "${usrbindir}/.debug ${usrsbindir}/.debug"
FILES_${PN} += "${base_libdir}/udev/*"
EXTRA_OEMAKE = "-e \
		'EXTRAS=${UDEV_EXTRAS}' \
		libudevdir=/lib/udev libdir=${base_libdir} prefix="

inherit update-rc.d

do_install () {
	install -d ${D}${usrsbindir} \
		   ${D}${usrbindir} \
		   ${D}${sbindir}
	oe_runmake 'DESTDIR=${D}' INSTALL=install install EXTRAS=extras/firmware
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/udev

	install -m 0755 ${WORKDIR}/create_udev_rule.sh  ${D}${sbindir}
	install -m 0755 ${WORKDIR}/udev_rules_awk       ${D}${sbindir}

	install -d ${D}${sysconfdir}/udev/rules.d/

	install -m 0644 ${WORKDIR}/mount.blacklist     ${D}${sysconfdir}/udev/
	install -m 0644 ${WORKDIR}/local.rules         ${D}${sysconfdir}/udev/rules.d/local.rules
	install -m 0644 ${WORKDIR}/permissions.rules   ${D}${sysconfdir}/udev/rules.d/permissions.rules
	install -m 0644 ${WORKDIR}/udev.rules          ${D}${sysconfdir}/udev/rules.d/udev.rules
	install -m 0644 ${WORKDIR}/links.conf          ${D}${sysconfdir}/udev/links.conf
	if [ "${UDEV_DEVFS_RULES}" = "1" ]; then
		install -m 0644 ${WORKDIR}/devfs-udev.rules ${D}${sysconfdir}/udev/rules.d/devfs-udev.rules
	fi

	install -d ${D}${sysconfdir}/udev/scripts/

	install -m 0755 ${WORKDIR}/mount.sh ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/network.sh ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/call_plugdev.sh ${D}${sysconfdir}/udev/scripts
	install -d ${D}${base_libdir}/udev/
    cd ${D}${sbindir}
    ln -f -s udevadm udevtrigger
    ln -f -s udevadm udevcontrol
}

do_install_append_acs6000() {
	install -m 0644 ${WORKDIR}/serial.rules ${D}${sysconfdir}/udev/rules.d/serial.rules
	install -m 0755 ${WORKDIR}/serial.sh ${D}${sbindir}
}
