SECTION = "base"
DESCRIPTION = "grand unified bootloader"

SRC_URI = "ftp://alpha.gnu.org/gnu/grub/grub-${PV}.tar.gz \
      file://fix-gcc-411.patch;patch=1 \
"

SRC_URI_append_mergepoint = " file://menu.diff;patch=1 "

SRC_URI_append_acs6k-x86 = "\
    file://device.map \
    file://grub.conf \
    file://install_acs_x86 \
    file://part_disk \
    "

SRC_URI_append_pmhd-x86 = "\
    file://device.map \
    file://grub.conf \
    file://install_pmhd_x86 \
    file://part_disk \
    "

SRC_URI_append_dsi5200 = "\
    file://device.map \
    file://grub.conf \
    file://splash.xpm.gz \
    file://caesar.patch;patch=1 \
    "

S = "${WORKDIR}/grub-${PV}"

inherit autotools

python __anonymous () {
    import re
    host = bb.data.getVar('HOST_SYS', d, 1)
    if not re.match('i.86.*-linux', host):
        raise bb.parse.SkipPackage("incompatible with host %s" % host)
}

FILES_${PN} += "/boot/*" 

do_install_append_acs6k-x86 () {
    install -d -m 0755 ${D}/boot/grub
    install -m 0644 ${WORKDIR}/device.map ${D}/boot/grub
    install -m 0644 ${WORKDIR}/grub.conf ${D}/boot/grub
    install -m 0755 ${WORKDIR}/install_acs_x86 ${D}/boot
    install -m 0755 ${WORKDIR}/part_disk ${D}/boot/
}

do_install_append_pmhd-x86 () {
    install -d -m 0755 ${D}/boot/grub
    install -m 0644 ${WORKDIR}/device.map ${D}/boot/grub
    install -m 0644 ${WORKDIR}/grub.conf ${D}/boot/grub
    install -m 0755 ${WORKDIR}/install_pmhd_x86 ${D}/boot
    install -m 0755 ${WORKDIR}/part_disk ${D}/boot/
}

do_install_prepend_aspen-dsi5200 () {
    sed -ir 's/console=tty.[0-9]/console=tty1/g' ${WORKDIR}/grub.conf
}

do_install_append_dsi5200 () {
    install -d -m 0755 ${D}/boot/grub
    install -m 0644 ${WORKDIR}/device.map ${D}/boot/grub
    install -m 0644 ${WORKDIR}/grub.conf ${D}/boot/grub

    # bitbake unziped splash.xpm.gz
    cat ${WORKDIR}/splash.xpm | gzip > ${D}/boot/grub/splash.xpm.gz
}

