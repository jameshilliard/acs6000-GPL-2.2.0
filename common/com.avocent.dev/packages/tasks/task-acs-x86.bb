DESCRIPTION = "Task packages for ACS 6000" 
MAINTAINER = "Regina Kodato <regina.kodato@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r0"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-acs-x86 \
    "

RDEPENDS_task-acs-x86 := "\
    grub \
    liblicensemgmt \
    vmlicensed \
#    avct-vm-serial \
#    axis-jre \
#    vmware-api \
    "
