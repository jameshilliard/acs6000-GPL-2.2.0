#
# cracklib.sh	create small dictionary for libcracklib.
#

if test ! -f /usr/lib/cracklib_dict.pwd 
then
	echo cracklib dictionary ...
	/usr/sbin/cracklib-format /usr/share/cracklib/cracklib-small | /usr/sbin/cracklib-packer /usr/lib/cracklib_dict >/dev/null 2>&1
fi

