DESCRIPTION = "Task packages for ACS 6000" 
MAINTAINER = "Regina Kodato <regina.kodato@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r0"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-acs-apps-ip \
    "

RDEPENDS_task-acs-apps-ip := "\
    terminal-menu \
    extra-rootfs \
    acs-vendor-mib \
    acs-temperature \
    dial-in \
    ci-serial \
    gsp-serial-daemon \
    cacpd-feval \
    avct-adsap2 \
    usbctl \
    firmware-upgrade \
    direct-connect \
    acs-sptest \
    "

RDEPENDS_task-acs-apps-ip_append_powerpc := "\
    gsp-serialnb \
    cli-migration \
    "

