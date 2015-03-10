DESCRIPTION = "Avocent Caesar Linux distribution Light"
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
ALLOW_EMPTY = "1"
PR = "r0"

PACKAGES = "\
    ireland-base-depends \
    ireland-task-base \
    ireland-task-settings \
    ireland-task-apps \
    ireland-task-apps-extra \
    ireland-task-lib-extra \
    ireland-task-connectivity \
    ireland-task-dbg "

RDEPENDS_ireland-base-depends := "\
	libgcc \
	libstdc++ \
	glibc-thread-db \
	glibc-extra-nss \
	"

RDEPENDS_ireland-task-base := "\
	ipkg \
	makedevs \
	util-linux \
	modutils \
	modutils-collateral \
	sysvinit \
	libpam \
	shadow \
	grub \
	e2tools \
	e2fsprogs-e2fsck \
	e2fsprogs-mke2fs \
	e2fsprogs-fsck \
	e2fsprogs-tune2fs \
	procps \
	bash \
	"

RDEPENDS_ireland-task-settings := "\
    "

# version number can be appended to package name
RDEPENDS_ireland-task-apps := "\
	less \
	xinetd \
	sudo \
	"

RDEPENDS_ireland-task-connectivity := "\
	net-tools \
	openssh-ssh \
	openssh-sshd \
	openssh-sftp \
	apache \
	bind-utils \ 
	ntpdate \
	"

RDEPENDS_ireland-task-apps-extra := "\
	perl \
	python \
	ruby \
	"

RDEPENDS_ireland-task-lib-extra := "\
#	libsoup \
#	libxml2 \
	"

RDEPENDS_ireland-task-dbg := "\
	ldd \
	strace \
	gdbserver \
	"
