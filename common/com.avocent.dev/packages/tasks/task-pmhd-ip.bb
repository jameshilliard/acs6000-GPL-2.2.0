DESCRIPTION = "Task packages for PM HD" 
MAINTAINER = "Livio Ceci <livio.ceci@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r0"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-pmhd-apps-ip \
    "

PACKAGES_append_pmhd_mpc8313 = "\
    task-pmhd-apps-machine \
    "

RDEPENDS_task-pmhd-apps-ip := "\
#    pmhd2rootfs \ deprecated
    extra-rootfs \
    pmhd-vendor-mib \
    pam-pmcli \
    pmcli \
    power-dlogd \
    sptest \
    cacpd-feval \
    avct-adsap2 \
    firmware-upgrade \
    "

RDEPENDS_task-pmhd-apps-machine := "\
    pmhd-display \
    libsensor \
    gsp-serialnb \
    "

