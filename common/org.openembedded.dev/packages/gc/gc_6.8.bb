DESCRIPTION="A garbage collector for C and C++"
SECTION="libs"
Maintainer = "Jiff Shen <jiff.shen@avocent.com>"
Homepage= "http://www.hpl.hp.com/personal/Hans_Boehm/gc/"
LICENCE = "unknown"

SRC_URI = "http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/gc${PV}.tar.gz"

S = ${WORKDIR}/gc${PV}

export SED = "sed"

inherit autotools

#PACKAGES = "gc-dbg gc-doc gc gc-dev"
FILES_${PN} = "${libdir}/lib*.so.*" 
FILES_${PN}-doc += "${datadir}/${PN}"

do_stage () {
	install -d ${STAGING_INCDIR}/gc  ${STAGING_INCDIR}/gc/private
	(cd include; for fn in $(find . -name "*.h"); \
		do install -m 0644 $fn ${STAGING_INCDIR}/gc/$fn; done )

	oe_libinstall -C .libs libgc ${STAGING_LIBDIR}
}
