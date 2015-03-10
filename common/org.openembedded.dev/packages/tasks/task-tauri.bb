DESCRIPTION = "Task packages for Tauri"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r1"

INHIBIT_IPK_UPLOAD = "1"

do_package[nostamp]="1"
do_populate_staging[nostamp]="1"
do_build[nostamp]="1"

PACKAGES = "\
    task-tauri-base-depends \
    task-tauri-base \
    task-tauri-apps \
    task-tauri-apps-extra \
    task-tauri-lib-extra \
    task-tauri-connectivity \
    task-tauri-utils \
    task-tauri-dbg \
    "

RDEPENDS_task-tauri-base-depends := "\
    "

RDEPENDS_task-tauri-base := "\
    modutils \
    modutils-collateral \
    unionfs-utils \
    termcap \
    ncurses \
    sysvinit \
    libpam \
    syslog-ng \
    shadow \
    wget \
    ipkg \
    util-linux \
    e2fsprogs-libs \
    e2fsprogs-e2fsck \
    e2fsprogs-mke2fs \
    e2fsprogs-fsck \
    e2fsprogs-tune2fs \
    bash \
    "


RDEPENDS_task-tauri-apps := "\
    openssh-sshd \
    procps \
    xinetd \
    ethtool \
    "

RDEPENDS_task-tauri-apps-extra := "\
    bind-utils \
    expect \
    sqlite3 \
    "


RDEPENDS_task-tauri-lib-extra := "\
    xerces-c \
    xalan-c \
    unixodbc \
#    libsoup \
#    libxml2 \
#    libxslt \
   "

RDEPENDS_task-tauri-connectivity := "\
    apache2  \
    net-tools \
    openssh-ssh \
    openssh-scp \
    openssh-sftp \
    vlan \
    iptables \
    iptables-utils\
    wide-dhcpv6 \
    tftp-hpa \
    netkit-ftp \
    "


RDEPENDS_task-tauri-utils := "\
    less \
    vim-tiny \
    "

# gdbserver provided by gdb
#
RDEPENDS_task-tauri-dbg := "\
    strace \
    gdbserver \
    "
