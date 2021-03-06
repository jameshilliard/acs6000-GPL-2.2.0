#@TYPE: Distribution
#@NAME: SlugOS
#@DESCRIPTION: SlugOS Linux Distribution for the NSLU2

# WARNING: this file should not be used directly, use one of
# the supported slugos-???.conf instead:

# Explanation of the naming
#
# slugos-dist - the base distro configuration for an NSLU2
# slugos-[bl][at][gu] - a specific configuration encoded as follows:
#
#  b or l - big or little endian
#         set ARCH_BYTE_SEX to match
#
#  a or t - ARM or Thumb instructions in the low level libraries
#         set ARM_INSTRUCTION_SET to match
#
#  g or u - glibc or uclibc libc implementation
#         set TARGET_OS to match
#
# The following names are deprecated:
#
# openslug - the historical name of a canonical slugos-bag distro
#            also the name of a standard slugos-bag distro with a package feed
# ucslugc  - the historical (and obsolete) name of slugos-btu
#            also the name of a standard slugos-btu distro with a package feed
#

#----------------------------------------------------------------------------------
# STANDARD SLUGOS DEFINITIONS
#----------------------------------------------------------------------------------
# The following must be set in the top level distro file:
#
#DISTRO_NAME
#DISTRO_TYPE

# The following may be overridden to make sub-versions
SLUGOS_VERSION = "4.0"
DISTRO_REVISION ?= ""
DISTRO_VERSION ?= "${SLUGOS_VERSION}${DISTRO_REVISION}-${DISTRO_TYPE}"
# For release (only):
# DISTRO_FEED ?= "${SLUGOS_VERSION}-${DISTRO_TYPE}"
DISTRO_FEED ?= "unstable"

# The following must also be set, they define the basic format of the build
#
#ARCH_BYTE_SEX "be" or "le"
#ARM_INSTRUCTION_SET "arm" or "thumb"
#TARGET_OS "linux" or "linux-uclibc"
#SLUGOS_IMAGESEX "big-endian" or "little-endian" (JFFS2 format)

# The following need not be set, the defaults should be adequate
#USE_NLS = "no" or "yes"
USE_NLS ?= "no"

# To make errors easier to identify:
DISTRO_NAME ?= "INVALID-${ARCH_BYTE_SEX}-${ARM_INSTRUCTION_SET}-${TARGET_OS}"
DISTRO_TYPE ?= "INVALID"

# The following must not be changed.
TARGET_FPU = "soft"

# glib-2.0 builds require USE_NLS to be overridden
USE_NLS_glib-2.0 = "yes"

INHERIT += "package_ipk debian"

DISTRO_BASE = ":slugos"

# packages/meta/slugos-image.bb generates a binary image of the root file system
# with a name controlled by the following setting - it defaults to the top level
# ${DISTRO_NAME} and this should always be correct, but it may be too specific (e.g.
# if you change the DISTRO from one of the standard ones in a way which does not
# change the generated image).
SLUGOS_IMAGENAME ?= "${DISTRO_NAME}"

# pull in the frozen list of bbfiles - this sets BBFILES and this will override any
# setting in local.conf  Since this depends on ${DISTRO} it doesn't actually do
# anything by default unless conf/distro/${DISTRO}-packages.conf exists.
# require conf/distro/include/freeze.conf

#----------------------------------------------------------------------------------
# FEEDS
#  The FEED_URIS lines define where on the internet SlugOS looks for packages
#  when downloading new packages or updating existing packages.  Additional feeds
#  (such as a local feed) can be defined in local.conf
#----------------------------------------------------------------------------------
# Add to the user's feeds from local.conf (there may be none).
# In general slugos-[bl][at][gu] is sufficient to identify a set of packages which
# will work on any derived distro.  The slugos-???.conf files set the FEED_URIS to
# an appropriate ipkg.nslu2-linux.org default if nslu2-linux is actually building
# such a feed:
#
#FEED_URIS_append += "cross##http://ipkg.nslu2-linux.org/feeds/${SLUGOS-foo}/cross/${DISTRO_FEED}"
#FEED_URIS_append += "native##http://ipkg.nslu2-linux.org/feeds/${SLUGOS-foo}/native/${DISTRO_FEED}"
#
# When such a feed exists the corresponding 'unstable' feed will normally also
# exist - the packages in unstable are the latest but very unlikely to be the
# greatest.  Use at your own risk!
# 
#FEED_URIS_append += "unstable_cross##http://ipkg.nslu2-linux.org/feeds/${SLUGOS-foo}/cross/unstable"
#FEED_URIS_append += "unstable_native##http://ipkg.nslu2-linux.org/feeds/${SLUGOS-foo}/native/unstable"

#----------------------------------------------------------------------------------
# FIRMWARE CONFIGURATION
#----------------------------------------------------------------------------------
# IMAGE BUILD OPTIONS
# -------------------
# CMDLINE* macros define the kernel command line.  CMDLINE_DEBUG is provided
# to enable/disable debugging in the distro.  Here noirqdebug is used to
# remove the messages about unhandled interrupts - people debugging interrupt
# issues in the kernel need to cancel this (set CMDLINE_DEBUG empty in local.conf)
CMDLINE_DEBUG = "noirqdebug"

# Building a full image.  If you set SLUGOS_FLASH_IMAGE to "yes" a complete
# 8MByte flash image will be built after the rootfs JFFS2 image.
#
# If you do this you will need to find a source for the LinkSys software to be
# placed in the full image - 'RedBoot' (the boot loader), 'Trailer' (just a bunch
# of bytes to flag the image as valid) and 'SysConf' (actually just a block of
# unset data - you could modify this to pre-configure the SysConf partition of the
# image).  If you own an NSLU2 then this data is already in the flash of the NSLU2
# and you can download it (including new versions) from the LinkSys web site - see
# packages/meta/slugos-image.bb for information about where to put the downloaded
# data.
SLUGOS_FLASH_IMAGE ?= "no"

# EXTRA PACKAGES
# --------------
# SlugOS builds an image with no extra packages - to add packages to the
# firmware include this file in a new 'distro' configuration file) and set
# the definition below to include the requirements.
# NOTE: as built this system will *not* be able to boot off anything other than
# the flash file system ('turnup' will not work).  You need to add the filesystem
# modules for your chosen root file system (and any others which will be used).
# See openslug.conf for a more full-featured system!
#
# Everything in this list gets added to the flash image, along with any RDEPENDS
# it has itself!
SLUGOS_EXTRA_RDEPENDS ?= ""

#----------------------------------------------------------------------------------
# PACKAGE VERSION CONTROL
#----------------------------------------------------------------------------------
# Define how to bootstrap the system - we need cross compilers.  It is also
# necessary to define which gets generated first, glibc or the kernel, and
# this is done by selecting the provider of libc (in this case it needs the
# kernel, so there has to be a non-libc gcc to build the kernel!)
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}gcc-initial:gcc-cross-initial"
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}gcc:gcc-cross"
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}g++:gcc-cross"
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}libc-for-gcc:glibc"
# Select 2.6 versions of the depmod support
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}depmod:module-init-tools-cross"
PREFERRED_PROVIDERS += "virtual/${TARGET_PREFIX}libc-for-gcc:glibc"

# compiler/binary/library versions (unslung uses different ones so these
# cannot be in ${MACHINE}.conf)
PREFERRED_VERSION_binutils ?= "2.16"
PREFERRED_VERSION_binutils-cross ?= "2.16"
PREFERRED_VERSION_gcc-cross-initial ?= "4.1.1"
PREFERRED_VERSION_gcc-cross ?= "4.1.1"
PREFERRED_VERSION_gcc ?= "4.1.1"
PREFERRED_VERSION_glibc ?= "2.3.5+cvs20050627"
#For SVN uclibc:
#SRCDATE_uclibc ?= "20060120"
#PREFERRED_VERSION_uclibc ?= "0.9.28+svn${SRCDATE_uclibc}"

# Select the correct versions of the kernel and modules (these are the
# defaults, override in the conf/distro top-level distro file).
PREFERRED_PROVIDER_virtual/kernel ?= "ixp4xx-kernel"
PREFERRED_VERSION_ixp4xx-kernel ?= "2.6.18"

# Select the smallest provider of x11 libraries
PREFERRED_PROVIDER_virtual/libx11 ?= "diet-x11"
SRCDATE_diet-x11 ?= "20060106"

# Latest wpa-supplicant which supports madwifi-ng
PREFERRED_VERSION_wpa-supplicant ?= "0.5.1"

# Ensure consistency across all SlugOS builds for the provider of a
# relational database - use the latest version of the SleepyCat
# Berkeley database
PREFERRED_PROVIDER_virtual/db = "db"
PREFERRED_PROVIDER_virtual/db-native = "db-native"

#FIXME: HACK: REMOVE THIS
IGNORE_STRIP_ERRORS = ""
