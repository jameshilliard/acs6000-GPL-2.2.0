DESCRIPTION = "Task packages for GSP" 
MAINTAINER = "Jiff Shen <jiff.shen@avocent.com>"
ALLOW_EMPTY = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"
PR = "r1"

PACKAGES = "\
    task-gsp-apps \
	"

RDEPENDS_task-gsp-apps := "\
	cracklib \
	pam-radius \
	pam-unix2 \
	pam-nis \
	pam-tacplus \
	pam-ldap \
	ypbind-mt \
	yp-tools \
	libxcrypt \
	krb5 \
	pam-krb5-fk\
	opie \
	pam-opie \
	smslink \
	libxml2 \
	net-snmp-server \
	net-snmp-client \
	timezone-data \
	ntpclient \
	iproute2 \
	socat \
	fw-utils \
	libcyevent \
	event-notify-headers \
	ncurses \
	ncurses-terminfo \
	"

# NOTE: To make the dependencies work TEMPORARILY, the feature modules are now listed
# in the DEPENDS variable in packages/typesafe-api/typesafe-api.bb.
