require xorg-lib-common.inc
PR = "r1"

SRC_URI += "file://fix-missing-includepath.patch;patch=1"

DESCRIPTION = "network API translation layer to \
insulate X applications and libraries from OS \
network vageries."

