DESCRIPTION = "Vim is an almost fully-compatible version of the Unix editor Vi."
SECTION = "console/utils"
DEPENDS = "ncurses"
LICENSE = "vim"

SRC_URI = "ftp://ftp.vim.org/pub/vim/unix/vim-${PV}.tar.bz2 \
	   file://configure.patch;patch=1"

S = "${WORKDIR}/vim${VIMVER}/src"

inherit autotools

do_configure () {
	rm -f auto/*
	touch auto/config.mk
	aclocal
        autoconf
	oe_runconf
	touch auto/configure
	touch auto/config.mk auto/config.h
}

PACKAGES =+ "vim-syntax vim-help vim-tutor vim-common"
FILES_vim-syntax = "${datadir}/vim/vim${VIMVER}/syntax"
FILES_vim-help = "${datadir}/vim/vim${VIMVER}/doc"
FILES_vim-tutor = "${datadir}/vim/vim${VIMVER}/tutor ${bindir}/vimtutor"
FILES_vim-common = "${datadir}/vim/vim${VIMVER}/"

pkg_postinst_${PN} () {
	update-alternatives --install ${base_bindir}/vi vi ${bindir}/vim 100
}

pkg_prerm_${PN} () {
	update-alternatives --remove vi ${bindir}/vim
}

#FILES_${PN} = "${bindir} ${sbindir} ${libexecdir} ${libdir}/lib*.so.* \
#	       ${libdir}/*/ ${sysconfdir} ${sharedstatedir} ${localstatedir} \
#	       /bin /sbin /lib/*/ /lib/*.so* ${datadir}/vim"
