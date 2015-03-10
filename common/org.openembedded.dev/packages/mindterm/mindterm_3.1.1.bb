DESCRIPTION = "mindterm 3.1.1 package"
HOMEPAGE = "http://www.appgate.com/products/80_MindTerm/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "java"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.appgate.com/products/80_MindTerm/110_MindTerm_Download/mindterm_${PV}-src.zip;localpath=${DL_DIR}/mindterm_${PV}-src.zip"

S=${WORKDIR}/mindterm_${PV}

PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/var/www/htdocs/mindterm* \
	"

do_compile() {
	${ANT_HOME}/bin/ant
}

do_install() {
	install -d ${D}/var/www/htdocs/
	install -m 0755 ${S}/mindterm.jar ${D}/var/www/htdocs/
}
