require glibc_${PV}.bb

FILESPATH = "${@base_set_filespath(['${FILE_DIRNAME}/glibc-${PV}', '${FILE_DIRNAME}/glibc', '${FILE_DIRNAME}/files', '${FILE_DIRNAME}' ], d)}"

DEFAULT_PREFERENCE = "-1"

do_install () {
	:
}

PACKAGES = ""
PROVIDES = "virtual/${TARGET_PREFIX}libc-for-gcc"
DEPENDS = "virtual/${TARGET_PREFIX}gcc-initial linux-libc-headers"
GLIBC_ADDONS = "nptl"
GLIBC_EXTRA_OECONF = ""
