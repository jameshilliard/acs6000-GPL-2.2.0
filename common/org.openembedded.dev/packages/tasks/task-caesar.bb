DESCRIPTION = "Avocent Caesar Linux distribution task"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
LICENSE = "MIT"
PR = "r1"

INHIBIT_IPK_UPLOAD = "1"

PACKAGES = "\
    task-caesar-base-depends \
    task-caesar-base \
    task-caesar-settings \
    task-caesar-apps \
    task-caesar-apps-extra \
    task-caesar-dtview \
    task-caesar-lib-extra \
    task-caesar-connectivity \
    task-caesar-dbg "

RDEPENDS_task-caesar-base-depends := "\
	libgcc \
	libstdc++ \
	glibc-thread-db \
	glibc-extra-nss \
	"

RDEPENDS_task-caesar-base := "\
	ipkg \
	makedevs \
	util-linux \
	modutils \
	modutils-collateral \
	sysvinit \
	libpam \
	shadow \
	termcap \
	grub \
	e2tools \
	e2fsprogs-e2fsck \
	e2fsprogs-mke2fs \
	e2fsprogs-fsck \
	e2fsprogs-tune2fs \
	procps \
	bash \
	"

RDEPENDS_task-caesar-settings := "\
    "

# version number can be appended to package name
RDEPENDS_task-caesar-apps := "\
	wget \
	vim \
	less \
	xinetd \
	sudo \
	"

RDEPENDS_task-caesar-connectivity := "\
	net-tools \
	bind-utils \ 
	ntpdate \
	openssh-ssh \
	openssh-sshd \
	openssh-sftp \
	tftp-hpa \
	apache \
	"

RDEPENDS_task-caesar-apps-extra := "\
	tcl \
	expect \
	unixodbc \
	postgresql \
	"

RDEPENDS_task-caesar-dtview := "\
	ruby \
	python \
	perl \
	samba \
    zip \
    unzip \
    subversion \
	"
RDEPENDS_task-caesar-lib-extra := "\
#	libsoup \
#	libxml2 \
	"

RDEPENDS_task-caesar-dbg := "\
	ldd \
	strace \
	gdbserver \
	"
