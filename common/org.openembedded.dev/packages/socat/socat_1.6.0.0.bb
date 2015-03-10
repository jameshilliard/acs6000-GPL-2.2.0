DESCRIPTION = "socat - Multipurpose relay"
SECTION = "base"
PRIORITY = "optional"
HOMEPAGE = "http://www.dest-unreach.org/socat/"

SRC_URI = "http://www.dest-unreach.org/socat/download/socat-1.6.0.0.tar.bz2 \
	   file://oe-build.patch;patch=1 \
	"
inherit autotools

do_configure() {
	./configure --host=${BUILD_SYS} \
                    --disable-termios \
                    --disable-openssl \
                    --prefix=/usr \
                    --cache-file=config.cache \
                    --exec-prefix=/usr \
                    LIBS="-L${STAGING_DIR}/${TARGET_SYS}/$/usr/lib -L${STAGING_DIR}/${TARGET_SYS}/lib"
}
