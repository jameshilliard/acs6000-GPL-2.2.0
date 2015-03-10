DESCRIPTION = "mindterm 3.1.2 package"
HOMEPAGE = "http://www.appgate.com/products/80_MindTerm/"
MAINTAINER = "Steven Yang <steven.yang@avocent.com>"
SECTION = "java"
PRIORITY = "optional"
LICENSE = ""
PR = "r0"

SRC_URI = "http://www.appgate.com/products/80_MindTerm/110_MindTerm_Download/mindterm_${PV}-src.zip;localpath=${DL_DIR}/mindterm_${PV}-src.zip"

SRC_URI_append_GSP = "\
                file://GSP.patch;patch=1 \
	"

S=${WORKDIR}/mindterm_${PV}

PACKAGES = "${PN}-dbg ${PN} ${PN}-doc ${PN}-dev ${PN}-locale"

FILES_${PN} = "\
	/var/www/htdocs/mindterm* \
	"

FILES_${PN}_GSP = "\
	/usr/share/apache2/htdocs/mindterm* \
	"

do_compile() {
	${ANT_HOME}/bin/ant
}

do_compile_amazon() {
	${ANT_HOME}/bin/ant
#	${ANT_HOME}/bin/ant clean mindterm.jar gencert sign_jar
}

do_compile_GSP() {
	${ANT_HOME}/bin/ant clean mindterm.jar gencert sign_jar
}

do_install() {
        install -d ${D}/var/www/htdocs/
        install -m 0755 ${S}/mindterm.jar ${D}/var/www/htdocs/
}

do_install_GSP() {
	install -d ${D}/usr/share/apache2/htdocs/
	install -m 0755 ${S}/build/mindterm.jar ${D}/usr/share/apache2/htdocs/mindterm-dev.jar
}









