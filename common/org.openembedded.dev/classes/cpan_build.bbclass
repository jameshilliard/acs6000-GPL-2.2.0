#
# This is for perl modules that use the new Build.PL build system
#
INHIBIT_NATIVE_STAGE_INSTALL = "1"
FILES_${PN} += '${libdir}/perl5'

DEPENDS  += "perl-native"
RDEPENDS += "perl"

#
# We also need to have built libmodule-build-perl-native for
# everything except libmodule-build-perl-native itself (which uses
# this class, but uses itself as the probider of
# libmodule-build-perl)
#
def cpan_build_dep_prepend(d):
    import bb;
    if bb.data.getVar('CPAN_BUILD_DEPS', d, 1):
        return ''
    pn = bb.data.getVar('PN', d, 1)
    if pn in ['libmodule-build-perl', 'libmodule-build-perl-native']:
        return ''
    return 'libmodule-build-perl-native '

DEPENDS_prepend = "${@cpan_build_dep_prepend(d)}"

def is_crosscompiling(d):
    import bb
    if not bb.data.inherits_class('native', d):
        return "yes"
    return "no"

cpan_build_do_configure () {
    if [ ${@is_crosscompiling(d)} == "yes" ]; then
        # build for target
        . ${STAGING_DIR}/${TARGET_SYS}/perl/config.sh
        perl Build.PL --installdirs vendor \
            --destdir ${D} \
            --install_path lib="${libdir}/perl5/site_perl/${version}" \
            --install_path arch="${libdir}/perl5/site_perl/${version}/${TARGET_SYS}" \
            --install_path script=${bindir} \
            --install_path bin=${bindir} \
            --install_path bindoc=${mandir}/man1 \
            --install_path libdoc=${mandir}/man3
    else
        # build for host
        perl Build.PL --installdirs site
    fi
}

cpan_build_do_compile () {
        perl Build
}

cpan_build_do_install () {
    if [ ${@is_crosscompiling(d)} == "yes" ]; then
        perl Build install
    fi
}

do_stage_append () {
    if [ ${@is_crosscompiling(d)} == "no" ]; then
        perl Build install
    fi
}

EXPORT_FUNCTIONS do_configure do_compile do_install
