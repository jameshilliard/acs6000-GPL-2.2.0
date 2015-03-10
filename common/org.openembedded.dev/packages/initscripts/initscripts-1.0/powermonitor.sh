#
# powermonitor.sh 
# Launch power failure monitor
#
if test -e /sbin/powerfailalarm.sh
then
	rm -f /tmp/.powerfailalarm.lock
	/sbin/powerfailalarm.sh > /dev/null 2>&1 &
fi
