#
# disable if vm
#
exit 0

#if test -e /sbin/monitorpowertemp.sh
#then
#	if test -e /var/run/.monitorpowertemp.lock
#	then
#		PID=`cat /var/run/.monitorpowertemp.lock`
#		kill -9 $PID >/dev/null 2>&1
#		rm -f /var/run/.monitorpowertemp.lock 
#	fi
#	/sbin/monitorpowertemp.sh > /dev/null 2>&1 &
#fi
