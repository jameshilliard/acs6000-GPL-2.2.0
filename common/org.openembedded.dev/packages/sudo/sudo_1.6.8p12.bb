SRC_URI = "http://ftp.sudo.ws/sudo/dist/sudo-${PV}.tar.gz \
	   file://nonrootinstall.patch;patch=1 \
           file://nostrip.patch;patch=1 \
	   file://autofoo.patch;patch=1 \
	   file://noexec-link.patch;patch=1 \
	   "
SRC_URI_append_acs6000 = " file://acs6000_dialout.patch;patch=1 "

#	   file://longlong.patch;patch=1

require sudo.inc
