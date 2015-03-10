#!/bin/sh
#
# monitorpowertemp.sh
#
# This shell script is for monitoring ACS 6000 
# power status for Dual-PowerSupply models, and 
# the temperature meassurements.
#
# When monitoring power status, the script will 
# turn ON alarm speaker of ACS 6000 board
# whenever it detects any power supply failure, and
# turn OFF alarm speaker as long as all power supplies
# are OK. And the script will send event notification.
#
# When monitoring the temperature sensor, the script 
# will read the configuration file with the MAX and
# the MIN values and the thresholds. And it will send
# event notification when the current temperature is:
# 1. greather than MAX+threshold
# 2. less than MIN-threshold
# 3. less than MAX-threshold after 1
# 4. greather than MIN+threshold after 2.
#

INTERVAL=3
LOCKFILE="/var/run/.monitorpowertemp.lock"
NUMBERPW=`cat /proc/sys/avocent/bdinfo/power`
LASTPW1=`cat /proc/sys/avocent/board/power1`
LASTPW2=`cat /proc/sys/avocent/board/power2`

lockfailed=0

. /etc/temperature_conf.ini

do_exit()
{
	if [ $lockfailed -eq 0 ] ; then
		# Turn OFF alarm speaker
		echo Stopping Monitor Power and Temperature sensor ...
		echo 0 > /proc/sys/avocent/board/buzzer
		rm -f $LOCKFILE 2>/dev/null
	fi
	exit 2
}

# send event notification about appliance power supply
sendpowerevent()
{
	# $1 is the power supply number
	# $2 is the current state 0-off 1-on
	echo "Power Supply "$1" is "$2
	if [ $2 -eq 0 ]; then
		/usr/bin/sh_write_evt evt_pow_stch iii $1 1 2
	else
		/usr/bin/sh_write_evt evt_pow_stch iii $1 2 1
	fi
}

checkpowerstatus()
{
	PW1=`cat /proc/sys/avocent/board/power1`
	PW2=`cat /proc/sys/avocent/board/power2`
    
	if [ "x$powerBuzzer" = "xenabled" ]; then
		CHECK=$[$PW1+$PW2];
		if [ $CHECK -ne 2 ]; then
			# Turn ON buzzer
			echo 1 > /proc/sys/avocent/board/buzzer
		else
			# Turn OFF buzzer
			echo 0 > /proc/sys/avocent/board/buzzer
		fi
	else
		# Turn OFF buzzer
		echo 0 > /proc/sys/avocent/board/buzzer
	fi

	if [ $LASTPW1 -ne $PW1 ]; then
		sendpowerevent 1 $PW1 
		LASTPW1=$PW1
	fi

	if [ $LASTPW2 -ne $PW2 ]; then
		sendpowerevent 2 $PW2 
		LASTPW2=$PW2
	fi
}


STATUS=0
MAX_UP=0
MAX_DOWN=0
MIN_DOWN=0
MIN_UP=0
CHECK_MAX=0
CHECK_MIN=0

if [ "x${maxValue}" != "x" ]; then
	CHECK_MAX=1
	MAX_UP=$[${maxValue}+${maxThreshold}]
	MAX_DOWN=$[${maxValue}-${maxThreshold}]
fi

if [ "x${minValue}" != "x" ]; then
	CHECK_MIN=1
	MIN_DOWN=$[${minValue}-${minThreshold}]	
	MIN_UP=$[${minValue}+${minThreshold}]
fi

EVT_MAX_UP="evt_temp_maxexd"
EVT_MAX_OK="evt_temp_maxrch"
EVT_MIN_UP="evt_temp_minexd"
EVT_MIN_OK="evt_temp_minrch"
 
checktemperature()
{
	val=`cat /sys/bus/i2c/devices/0-0048/temp1_input`
	value=$[${val}/1000]
	
	if [ ${STATUS} -eq 0 ]; then
		if [ ${CHECK_MIN} -eq 1 -a ${value} -lt ${MIN_DOWN} ]; then
			STATUS=1
			/usr/bin/sh_write_evt ${EVT_MIN_UP} iii ${minValue} ${minThreshold} ${value}
		fi
		if [ ${CHECK_MAX} -eq 1 -a ${value} -gt ${MAX_UP} ]; then
			STATUS=2
			/usr/bin/sh_write_evt ${EVT_MAX_UP} iii ${maxValue} ${maxThreshold} ${value}
		fi
		return
	fi
	if [ ${STATUS} -eq 1 ]; then
		if [ ${value} -ge ${MIN_UP} ]; then
			STATUS=0
			/usr/bin/sh_write_evt ${EVT_MIN_OK} iii ${minValue} ${minThreshold} ${value}
		fi
		return	
	fi
	if [ ${STATUS} -eq 2 ]; then
		if [ ${value} -le ${MAX_DOWN} ]; then
			STATUS=0
			/usr/bin/sh_write_evt ${EVT_MAX_OK} iii ${maxValue} ${maxThreshold} ${value}
		fi
	fi
}

trap do_exit EXIT SIGTERM SIGINT SIGQUIT SIGHUP

# we only need one instance of the shell script running.
if [ -a $LOCKFILE ]; then 
	lockfailed=1
	echo "Error: It has been already started."
	echo "Please check LOCKFILE $LOCKFILE"
	exit
else
	echo $$ > ${LOCKFILE}
fi

while [ 1 -eq 1 ] ; do
	if [ $NUMBERPW -eq 2 ]; then
		checkpowerstatus
	fi
	if [ $CHECK_MAX -eq 1 -o $CHECK_MIN -eq 1 ]; then
		checktemperature
	fi
	sleep $INTERVAL
	sync
done
