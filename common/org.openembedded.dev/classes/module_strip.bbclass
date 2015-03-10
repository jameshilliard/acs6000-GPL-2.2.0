#DEPENDS_append = " module-strip"

do_strip_modules () {
    for p in ${PACKAGES}; do
        if test -e ${WORKDIR}/install/$p/lib/modules; then
            modules="`find ${WORKDIR}/install/$p/lib/modules -name \*${KERNEL_OBJECT_SUFFIX}`"
            if [ -n "$modules" ]; then
                for module in $modules ; do
                    if ! [ -d "$module"  ] ; then
                        ${STRIP} -v -g $module
                    fi
                done    
#                NM="${CROSS_DIR}/bin/${HOST_PREFIX}nm" OBJCOPY="${CROSS_DIR}/bin/${HOST_PREFIX}objcopy" strip_module $modules
            fi
        fi
    done
}

python __anonymous () {
    import bb, re
    if (bb.data.getVar('INHIBIT_PACKAGE_STRIP', d, 1) != '1'):
        pkgfuns = bb.data.getVar ('PACKAGEFUNCS', d, 1)
        pkgfuns = re.sub (r""" (do_package_\w+)""", r" do_strip_modules \1", pkgfuns)
        bb.data.setVar ('PACKAGEFUNCS', pkgfuns, d)
}
