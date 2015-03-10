DESCRIPTION = "Core packages required for a basic installation"
MAINTAINER = "Phil Blundell <pb@handhelds.org>"
PR = "r25"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

# The BOOTSTRAP_EXTRA_ variables are often manipulated by the
# MACHINE .conf files, so adjust PACKAGE_ARCH accordingly.
PACKAGE_ARCH = "${MACHINE_ARCH}"

ALLOW_EMPTY = 1
PACKAGES = "${PN}"

MODUTILS ?= "24 26"

INHIBIT_IPK_UPLOAD = "1"

def bootstrap_modutils_rdepends(d):
    import bb
    m = bb.data.getVar('MODUTILS', d, 1)
    r = []
    if '24' in m:
        r.append('modutils-depmod')
    if '26' in m:
        r.append('module-init-tools-depmod')
    return ' '.join(r)

HOTPLUG ?= "linux-hotplug"

HOTPLUG_shasta = ""
HOTPLUG_acs6000 = ""
HOTPLUG_pmhd = ""

RDEPENDS = 'base-files base-passwd busybox \
	initscripts \
	netbase sysvinit sysvinit-pidof \
	modutils-initscripts setserial\
	${HOTPLUG} \
	${BOOTSTRAP_EXTRA_RDEPENDS} \
	${@bootstrap_modutils_rdepends(d)}'

RDEPENDS_acs6000 = 'base-files base-passwd busybox \
	initscripts \
	netbase sysvinit sysvinit-pidof \
	setserial \
	${BOOTSTRAP_EXTRA_RDEPENDS} \
	'

RDEPENDS_pmhd = 'base-files base-passwd busybox \
	initscripts \
	netbase sysvinit sysvinit-pidof \
	${BOOTSTRAP_EXTRA_RDEPENDS} \
	'

RRECOMMENDS_amazon = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS = 'dropbear portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS_tauri = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS_caesar = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS_acs6000 = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS_pmhd = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

RRECOMMENDS_shasta = 'portmap \
	${BOOTSTRAP_EXTRA_RRECOMMENDS}'

LICENSE = MIT
