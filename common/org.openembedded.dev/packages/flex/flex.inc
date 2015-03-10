DESCRIPTION = "Flex is a tool for generating programs that recognize lexical patterns in text."
SECTION = "devel"
LICENSE = "BSD"

SRC_URI = "${SOURCEFORGE_MIRROR}/lex/flex-${PV}.tar.bz2 \
           file://flex-lvalue.diff;patch=1 \
           file://fix-gen.patch;patch=1"
S = "${WORKDIR}/flex-${PV}"

inherit autotools

