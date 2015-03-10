#
# remount.sh	remount filesystem.
#
# Version:	@(#)remount.sh  0.01  23-May-2007 lynn.lin@avocent.com
#
. /etc/default/rcS
mount -n -o remount,rw /
