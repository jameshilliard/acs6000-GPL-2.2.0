DESCRIPTION = "Task packages for ACS 6000" 
MAINTAINER = "Regina Kodato <regina.kodato@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r0"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-acs-base-depends \
    task-acs-base \
    task-acs-fs \
    task-acs-apps \
    task-acs-apps-extra \
    task-acs-lib-extra \
    task-acs-connectivity \
    task-acs-utils \
    task-acs-dbg \
    "

RDEPENDS_task-acs-base-depends := "\
    openssl \
    zlib \
    wireless-tools \
    udev \
    pciutils \
    pciutils-ids \
    usbutils \
    coreutils \
    devices-fw \
    "

RDEPENDS_task-acs-base-depends_append_powerpc := "\
    smusb \
	"

RDEPENDS_task-acs-base := "\
    module-init-tools \
    unionfs-utils \
    termcap \
    minicom \
    sysvinit \
    libpam \
    shadow \
    wget \
    ipkg \
    util-linux \
    bash \
    mgetty \
    sudo \
    lrzsz \
    "

RDEPENDS_task-acs-fs := "\
    e2fsprogs-libs \
    e2fsprogs-e2fsck \
    e2fsprogs-mke2fs \
    e2fsprogs-fsck \
    e2fsprogs-tune2fs \
    dosfstools \
    "

RDEPENDS_task-acs-fs_append_powerpc := "\
    mtd-utils \
    "

RDEPENDS_task-acs-apps := "\
    openssh-sshd \
    net-snmp-server \
    net-snmp-libs \
    net-snmp-client \
    xinetd \
    ethtool \
    procps \
    "

RDEPENDS_task-acs-connectivity := "\
    net-tools \
    openssh-ssh \
    openssh-scp \
    openssh-sftp \
    iproute2 \
    openswan \
    vlan \
    iptables \
    iptables-utils \
    wide-dhcpv6 \
    tftp-hpa \
    netkit-ftp-usagi \
    linux-ftpd-usagi \
    apache2 \
    bcm-fwcutter \
    curl \
    ppp \
    ppp-dialin \
    "

RDEPENDS_task-acs-apps-extra := "\
    bind-utils \ 
    postgresql \
    expect \
    "

RDEPENDS_task-acs-lib-extra := "\
    unixodbc \
    "

RDEPENDS_task-acs-utils := "\
    less \
    vim-tiny \
    "

# gdbserver provided by gdb
#
RDEPENDS_task-acs-dbg := "\
    strace \
    gdbserver \
    "

