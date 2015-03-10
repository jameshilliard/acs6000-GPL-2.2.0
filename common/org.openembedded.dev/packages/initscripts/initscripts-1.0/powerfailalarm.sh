#!/bin/sh
#
# powerfailalarm.sh
#
# This shell script is for monitoring mergepoint 5224/5240
# power status for Dual-PowerSupply models, for
# Single-PowerSupply models, this script does NOTHING.
#
# When monitoring cyclades board power status, the
# script will turn ON alarm speaker of 5224/5240 board
# whenever it detects any power supply failure, and
# turn OFF alarm speaker as long as all power supplies
# are OK.
#
# The working theory for Cyclades' OnBoard is:
#  1. First check board_model in /proc/bdinfo's 16th field
#     to ensure it's a DualPS board.
#  2. For DualPS, every INTERVAL seconds, check /proc/powerstatus
#  3. According to powerstatus, turn ON/OFF 5224/5240 's alarm
#     speaker by echo 1/0 to /proc/sys/speaker/speaker
#
#

INTERVAL=5
LOGFILE="/var/log/powerfailalarm.log"
LOCKFILE="/var/run/.powerfailalarm.lock"
DUALPWRONSTRING=`cat /proc/powerstatus | sed "s/OFF/ON/g"`
lockfailed=0

cleanup()
{
    if [ $lockfailed -eq 0 ] ; then
	# Turn OFF alarm speaker whenever exit no matter normally or abnormally.
	echo Stopping Power Fail Alarm System...
	echo 0 > /proc/sys/speaker/speaker
	echo `date +"%Y%m%d %T"` " Power Fail Alarm System Stopped." >> $LOGFILE
	rm -f $LOCKFILE 2>/dev/null
    fi
}

#
# board_model is an unsigned byte integer with value 0 to 255.
#    MSB is 24/40 port bit; 1 for 24, 0 for 40
#    next bit is SinglePS/DualPS; 1 for Single, 0 for Dual
#    
checkdualps()
{
    board_model=`cat /proc/bdinfo | cut -f 16 -d \!`
    singleps=`expr $board_model / 64 % 2`

    if [ $singleps -eq 1 ] ; then
	echo This is a Single Power Supply model.
	echo `date +"%Y%m%d %T"` " This is a Single Power Supply model." >> $LOGFILE
	exit 0
    else
	echo This is a Dual Power Supply model.
	echo `date +"%Y%m%d %T"` " This is a Dual Power Supply model." >> $LOGFILE
    fi
}

#
# update $LOGFILE with new status in $1 if
# it is different from the status in the last
# log line of $LOGFILE.
# 
updatelog()
{
    if [ $# -ne 1 ] ; then
	return
    fi

    if [ -f $LOGFILE ] ; then
	lastlog=`tail -1 $LOGFILE | sed "s/^.*PowerA/PowerA/"`
	if [ "$1" != "$lastlog" ] ; then
	    echo `date +"%Y%m%d %T"` " $1" >> $LOGFILE
	fi
    fi
}

#
# main function of the PowerFailAlarm, runs
# every $INTERVAL seconds.
#
checkpowerstatus()
{
    powerstatus=`cat /proc/powerstatus`
    withoff=`echo $powerstatus | grep OFF`
    
    if [ -z "$withoff" ] ; then
	# Turn OFF alarm speaker
	echo 0 > /proc/sys/speaker/speaker
 	updatelog "$powerstatus"
    else
	# Turn ON alarm speaker
	echo 1 > /proc/sys/speaker/speaker
 	updatelog "$powerstatus"
    fi
}

trap cleanup EXIT

echo Starting Power Fail Alarm System ...

# we only need one instance of the shell script running.
lockfile -1 -r 1 $LOCKFILE > /dev/null 2>&1
if [ $? -ne 0 ] ; then
    echo "Error: It has been already started."
    echo "Please check LOGFILE $LOGFILE and LOCKFILE $LOCKFILE"
    lockfailed=1
    exit
fi


echo `date +"%Y%m%d %T"` " Power Fail Alarm System Started." >> $LOGFILE
checkdualps

while [ 1 -eq 1 ] ; do
    checkpowerstatus
    sleep $INTERVAL
done
    
