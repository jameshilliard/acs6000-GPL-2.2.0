MODULE_NAME = pam-nis

SRC_URI = "${AVCT_GSP_SVN}/sources/generic;module=${MODULE_NAME};localpath=${DL_DIR}/${MODULE_NAME} \
	file://configure-fix.patch;patch=1"

S="${WORKDIR}/${PN}"

CFLAGS_append = " -Wall -Werror"

inherit autotools pkgconfig srclnk

DEPENDS="libxcrypt"
EXTRA_OECONF = "--enable-xcrypt"

FILES_${PN} += "/lib/security/*.so*"

do_install_append() {
	chmod u+s ${D}/usr/sbin/pam-nis-helper
}

