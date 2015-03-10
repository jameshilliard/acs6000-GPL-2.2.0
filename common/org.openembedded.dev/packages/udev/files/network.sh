#!/bin/sh

###### un-comment to debug ######
#set -x
#exec > /dev/ttyS0 2>&1
#echo $0: called $INTERFACE
#env

export PATH=/sbin:$PATH

# Do not run when pcmcia-cs is installed
test -x /sbin/cardctl && exit 0

# We get two "add" events for hostap cards due to wifi0
echo "$INTERFACE" | grep -q -e "wifi\|wmaster0" && exit 0


# Check if /etc/init.d/network has been run yet to see if we are 
# called by starting /etc/rcS.d/S03udev and not by hotplugging a device
#
# At this stage, network interfaces should not be brought up
# automatically because:
# a)	/etc/init.d/network has not been run yet (security issue)
# b) 	/var has not been populated yet so /etc/resolv,conf points to 
#	oblivion, making the network unusable
#

spoofp="`grep ^spoofprotect /etc/network/options`"
if test -z "$spoofp"
then
	# This is the default from /etc/init.d/network
	spoofp_val=yes
else
	spoofp_val=${spoofp#spoofprotect=}
fi

test "$spoofp_val" = yes && spoofp_val=1 || spoofp_val=0

# I think it is safe to assume that "lo" will always be there ;)
if test "`cat /proc/sys/net/ipv4/conf/lo/rp_filter`" != "$spoofp_val" -a -n "$spoofp_val"
then
	echo "$INTERFACE" >> /dev/udev_network_queue	
	exit 0
fi

#
# Code taken from pcmcia-cs:/etc/pcmcia/network
#

# if this interface has an entry in /etc/network/interfaces, let ifupdown
# handle it
if grep -q "iface \+$INTERFACE" /etc/network/interfaces; then
  case $ACTION in
    add)
	  bus=$(/usr/bin/udevinfo -a -p ${DEVPATH} | grep SUBSYSTEMS | grep -e "usb\|pcmcia\|ide\|pci" | tail -n 1 | awk -F "==" '{print $2}')
      case ${bus} in
        \"pcmcia\")
          PHYSDEVBUS="pcmcia"
          PHYSDEVPATH=${DEVPATH%/*/*}
        ;;
        \"ide\")
          PHYSDEVBUS="ide"
          ;;
        \"usb\")
          if [ "$MINOR" == "" ] || [ "$MINOR" == "0" ] ; then
             PHYSDEVBUS="usb"
             PHYSDEVPATH=${DEVPATH%/usb*/*}
          else
             exit 0
          fi
        ;;
        \"pci\")
          PHYSDEVBUS="pci"
          PHYSDEVPATH=${DEVPATH%/*/*}
        ;;
        *)
          echo "$0: could not recognise device"
          exit 1
        ;;
      esac
      ifconfig | grep -q "^$INTERFACE"
      if [ $? -ne 0 ] ; then
        ifup $INTERFACE &
      fi
    ;;
    remove)
	  existcheck=`cat /tmp/plugdev/devicename | grep -c ${INTERFACE}`
      if [ "${existcheck}" -ne "0" ] ; then
        ifdown $INTERFACE
      fi
    ;;
  esac
  
  exit 0
fi
