DESCRIPTION = "Task packages for PM HD" 
MAINTAINER = "Livio Ceci <livio.ceci@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r0"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-pmhd-base-depends \
    task-pmhd-base \
    task-pmhd-fs \
    task-pmhd-apps \
    task-pmhd-apps-extra \
    task-pmhd-connectivity \
    task-pmhd-utils \
    task-pmhd-dbg \
    "

RDEPENDS_task-pmhd-base-depends := "\
    openssl \
    udev \
    udev-utils \
    "

RDEPENDS_task-pmhd-base := "\
    module-init-tools \
    unionfs-utils \
    termcap \
    sysvinit \
    libpam \
    shadow \
    wget \
    ipkg \
    util-linux \
    bash \
    sudo \
    lrzsz \
    "

RDEPENDS_task-pmhd-fs := "\
    e2fsprogs-libs \
    e2fsprogs-e2fsck \
    e2fsprogs-mke2fs \
    e2fsprogs-fsck \
    e2fsprogs-tune2fs \
    dosfstools \
    "

RDEPENDS_task-pmhd-fs_append_powerpc := "\
    mtd-utils \
    "

RDEPENDS_task-pmhd-apps := "\
    openssh-sshd \
    net-snmp-server \
    net-snmp-libs \
    net-snmp-client \
    xinetd \
    ethtool \
    minicom \
    "

RDEPENDS_task-pmhd-apps_append_powerpc := "\
    procps \
    "

RDEPENDS_task-pmhd-connectivity := "\
    net-tools \
    openssh-ssh \
    openssh-scp \
    openssh-sftp \
    iproute2 \
    vlan \
    iptables \
    iptables-utils \
    wide-dhcpv6 \
    tftp-hpa \
    netkit-ftp-usagi \
    linux-ftpd-usagi \
    apache2 \
    "

RDEPENDS_task-pmhd-apps-extra := "\
    bind-utils \ 
    expect \
    "

RDEPENDS_task-pmhd-utils := "\
    less \
    vim-tiny \
    "

# gdbserver provided by gdb
#
RDEPENDS_task-pmhd-dbg := "\
    strace \
    gdbserver \
    "

