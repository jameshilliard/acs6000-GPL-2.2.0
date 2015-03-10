# Find any machine specific sub packages and if present, mark the 
# whole package as machine specific for multimachine purposes.
def multimach_arch (d):
    import bb
    packages = bb.data.getVar('PACKAGES', d, 1).split()
    macharch = bb.data.getVar('MACHINE_ARCH', d, 1)
    multiarch  = bb.data.getVar('PACKAGE_ARCH', d, 1)

    for pkg in packages:
        pkgarch = bb.data.getVar("PACKAGE_ARCH_%s" % pkg, d, 1)

        # We could look for != PACKAGE_ARCH here but how to choose 
        # if multiple differences are present?
        # Look through IPKG_ARCHS for the priority order?
        if pkgarch and pkgarch == macharch:
            multiarch = macharch

    return multiarch

MULTIMACH_ARCH = "${@multimach_arch(d)}"
STAMP = "${TMPDIR}/stamps/${MULTIMACH_ARCH}${TARGET_VENDOR}-${TARGET_OS}/${PF}"
WORKDIR = "${TMPDIR}/work/${MULTIMACH_ARCH}${TARGET_VENDOR}-${TARGET_OS}/${PF}"
STAGING_KERNEL_DIR = "${STAGING_DIR}/${MULTIMACH_ARCH}${TARGET_VENDOR}-${TARGET_OS}/kernel"

