#!/bin/sh

if ! [ -f /var/run/cacpd.pid ]; then
    exit 0
fi

FILE_LOCK=/var/run/call_plugdev.sh.pid
#exec > /dev/console 2>&1
#env
#set -x

if [ -f $FILE_LOCK ] ; then
	while kill -0 `cat $FILE_LOCK` 2>/dev/null;do
		echo "call_plugdev.sh still running."
		sleep 1
	done
fi
echo $$ > $FILE_LOCK
# We get two "add" events for hostap cards due to wifi0
echo "$INTERFACE" | grep -q wifi && exit 0

echo "$0 called, env:"

if [ "${ACTION}" == "add" ]
then
	bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"pcmcia\""`
	if [ "${bus}" -ne "0" ] && [ "${PHYSDEVBUS}" != "ide" ]
	then
		PHYSDEVBUS="pcmcia"
		PHYSDEVPATH=${DEVPATH%/*/*}
	else
		bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"ide\""`
		if [ "${bus}" -ne "0" ]
		then
			PHYSDEVBUS="ide"
		else
			bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"usb\""`
			firstdev=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "ATTRS{port_number}==\"0\""`
			if [ "${bus}" -ne "0" ]
			then
				if [ "$MINOR" == "" ] || [ "$MINOR" == "0" ] || [ "${firstdev}" -ne "0" ]
				then	
					PHYSDEVBUS="usb"
					PHYSDEVPATH=${DEVPATH%/usb*/*}
				else
					if [ "$DEVTYPE" == "disk" ]
					then
						PHYSDEVBUS="usb"
					else
						exit 0
					fi
				fi			
			else
				bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"pci\""`
				if [ "${bus}" -ne "0" ]
				then
					PHYSDEVBUS="pci"
					PHYSDEVPATH=${DEVPATH%/*/*}
				else
					echo "$0 could not recognise device"

				fi
			fi
		fi
	fi
	if [ "${PHYSDEVBUS}" == "pcmcia" -o "${PHYSDEVBUS}" == "pci" ] || [ "${PHYSDEVBUS}" == "ide" ] || [ "${PHYSDEVBUS}" == "usb" ]
	then
		sockpath=${PHYSDEVPATH%/*}
		
		if ! [ -d /tmp/plugdev ]; then
   			mkdir /tmp/plugdev
		fi

		cd /tmp/plugdev

		case ${SUBSYSTEM} in
		net)
		dir=${sockpath##*/}
		slot=`expr ${dir#*.} + 1`
		if [ "${PHYSDEVBUS}" == "pci" ]
		then
			if [ "${dir#*.}" == "0" ]
			then
				infoprod=`/usr/sbin/lspci | grep "01:00.0"`
				eventprod=`/usr/sbin/lspci | grep "01:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
			else
				infoprod=`/usr/sbin/lspci | grep "05:00.0"`
                                eventprod=`/usr/sbin/lspci | grep "05:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
			fi
		else
			if [ "${PHYSDEVBUS}" == "usb" ]
			then
				bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"pci\""`
				if [ "${bus}" -ne "0" ]
				then
					slot="usbpci"
				else 
					slot="usbslot"
				fi
				infoprod=" product info: ${ID_SERIAL}"
				eventprod=${ID_SERIAL}
			else
				infoprod=`/sbin/pccardctl ident ${dir#*.} | grep "product"`
				eventprod=`/sbin/pccardctl ident ${dir#*.} | grep "product" | cut -d':' -f2- | sed 's/\,//g'`
			fi
		fi
		echo "${INTERFACE}" >> "devicename"
		echo "${INTERFACE} Ethernet ${slot} ${SUBSYSTEM} ${PHYSDEVBUS}" >> "deviceinfo"
		echo "${INTERFACE} ${infoprod}" >> "prodinfo"
		sed "/^${INTERFACE}/d" "/tmp/plugdev/ejected_devices" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/ejected_devices
		sed "/^${INTERFACE}/d" "/tmp/plugdev/ejected_devicename" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/ejected_devicename
		sh_write_evt evt_plgd_ins sss "${INTERFACE}" Ethernet "${eventprod}"

		if cat /proc/net/wireless | grep ${INTERFACE}
		then
			exec plugdev wireless ${INTERFACE}
		else
			exec plugdev net ${INTERFACE}
		fi
		 ;;
		tty)
                dir=${sockpath##*/}
                slot=`expr ${dir#*.} + 1`
		if [ "${PHYSDEVBUS}" == "pci" ]
		then
		     	if [ "${dir#*.}" == "0" ]
                        then
                                infoprod=`/usr/sbin/lspci | grep "01:00.0"`
                                eventprod=`/usr/sbin/lspci | grep "01:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
                        else
                                infoprod=`/usr/sbin/lspci | grep "05:00.0"`
                                eventprod=`/usr/sbin/lspci | grep "05:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
                        fi
		else
			if [ "${PHYSDEVBUS}" == "usb" ]
			then
				bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"pci\""`
				if [ "${bus}" -ne "0" ]
				then
					slot="usbpci"
				else
					slot="usbslot"
				fi
				infoprod=" product info: ${ID_SERIAL}"
				eventprod=${ID_SERIAL}
			else
				infoprod=`/sbin/pccardctl ident ${dir#*.} | grep "product"`
				eventprod=`/sbin/pccardctl ident ${dir#*.} | grep "product" | cut -d':' -f2- | sed 's/\,//g'`	
			fi
		fi
		echo "${DEVNAME##*/}" >> "devicename"
		echo "${DEVNAME##*/} Modem ${slot} ${SUBSYSTEM} ${PHYSDEVBUS}" >> "deviceinfo"
		echo "${DEVNAME##*/} ${infoprod}" >> "prodinfo"
		sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/ejected_devices" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/ejected_devices
		sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/ejected_devicename" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/ejected_devicename
		sh_write_evt evt_plgd_ins sss "${DEVNAME##*/}" Modem "${eventprod}"

		exec plugdev tty ${DEVNAME##*/}
	 	 ;;
		block)
		if [ "$MINOR" == "" ] || [ "$MINOR" == "0" ] || [ "$DEVTYPE" == "disk" ] 
		then
			if [ "${PHYSDEVBUS}" == "usb" ]
                        then
                        	bus=`/usr/bin/udevinfo -a -p ${DEVPATH} | grep -c "SUBSYSTEMS==\"pci\""`
                                if [ "${bus}" -ne "0" ]
                                then
                                	slot="usbpci"
                                else
                                	slot="usbslot"
                                fi
                        	infoprod=" product info: ${ID_SERIAL}"
                                eventprod=${ID_SERIAL}
			else
                      		dir=`echo ${PHYSDEVPATH} |awk -F/ '{print $4}'`
                        	slot=`expr ${dir#*.} + 1`
				infoprod=`/sbin/pccardctl ident ${dir#*.} | grep -c "no product"`
               
				if [ "${infoprod}" -ne "0" ]
				then 
					if [ "${dir#*.}" == "0" ]
		                        then
                		                infoprod=`/usr/sbin/lspci | grep "01:00.0"`
                                		eventprod=`/usr/sbin/lspci | grep "01:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
                        		else
                                		infoprod=`/usr/sbin/lspci | grep "05:00.0"`
                                		eventprod=`/usr/sbin/lspci | grep "05:00.0" | cut -d' ' -f2- | sed 's/\,//g'`
                        		fi
				else
					infoprod=`/sbin/pccardctl ident ${dir#*.} | grep "product info:"`
					eventprod=`/sbin/pccardctl ident ${dir#*.} | grep "product info:" | cut -d':' -f2- | sed 's/\,//g'`
				fi
			fi
			echo "${DEVNAME##*/}" >> "devicename"
                	echo "${DEVNAME##*/} Storage ${slot} ${SUBSYSTEM} ${PHYSDEVBUS}" >> "deviceinfo"
			sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/ejected_devices" > /tmp/temp
			mv -f /tmp/temp /tmp/plugdev/ejected_devices
			sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/ejected_devicename" > /tmp/temp
			mv -f /tmp/temp /tmp/plugdev/ejected_devicename
                	echo "${DEVNAME##*/} ${infoprod}" >> "prodinfo"
			sh_write_evt evt_plgd_ins sss "${DEVNAME##*/}" Storage "${eventprod}"

			# Adjust Data Buffering Directories
			(sleep 1; /etc/init.d/dbufd setdir) &

			echo plugdev block ${DEVNAME##*/}
		fi
		 ;;
		esac
	fi
fi
if [ "${ACTION}" == "remove" ]
then
	case ${SUBSYSTEM} in
	net)
	existcheck=`cat /tmp/plugdev/devicename | grep -c ${INTERFACE}`
	if [ "${existcheck}" -ne "0" ]
	then
		sed "/^${INTERFACE}/d" "/tmp/plugdev/devicename" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/devicename
		sed  -n "/^${INTERFACE}/p" "/tmp/plugdev/deviceinfo" >> /tmp/plugdev/ejected_devices
		sed "/^${INTERFACE}/d" "/tmp/plugdev/deviceinfo" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/deviceinfo
		sed "/^${INTERFACE}/d" "/tmp/plugdev/prodinfo" > /tmp/temp
		eventprod=`cat /tmp/plugdev/prodinfo | grep "${INTERFACE}" | cut -d' ' -f3- | sed 's/\,//g'`
		mv -f /tmp/temp /tmp/plugdev/prodinfo
		echo "${INTERFACE}" >> /tmp/plugdev/ejected_devicename
		sh_write_evt evt_plgd_ej sss "${INTERFACE}" Ethernet "${eventprod}"
	fi
	 ;;
	tty)
	existcheck=`cat /tmp/plugdev/devicename | grep -c ${DEVNAME##*/}`
	if [ "${existcheck}" -ne "0" ]
	then
		sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/devicename" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/devicename
		sed -n "/^${DEVNAME##*/}/p" "/tmp/plugdev/deviceinfo" >> /tmp/plugdev/ejected_devices
		sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/deviceinfo" > /tmp/temp
		mv -f /tmp/temp /tmp/plugdev/deviceinfo
		sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/prodinfo" > /tmp/temp
		eventprod=`cat /tmp/plugdev/prodinfo | grep "${DEVNAME##*/}" | cut -d' ' -f3- | sed 's/\,//g'`
		mv -f /tmp/temp /tmp/plugdev/prodinfo
		echo "${DEVNAME##*/}" >> /tmp/plugdev/ejected_devicename
		sh_write_evt evt_plgd_ej sss "${DEVNAME##*/}" Modem "${eventprod}"
		exec plugdev ttyremove ${DEVNAME##*/}
	fi
	 ;;
	block)
	existcheck=`cat /tmp/plugdev/devicename | grep -c ${DEVNAME##*/}`
        if [ "${existcheck}" -ne "0" ]
	then
		if [ "$MINOR" == "0" ]
		then
			sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/devicename" > /tmp/temp
			mv -f /tmp/temp /tmp/plugdev/devicename
#			sed  -n "/^${DEVNAME##*/}/p" "/tmp/plugdev/deviceinfo" >> /tmp/plugdev/ejected_devices
			sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/deviceinfo" > /tmp/temp
			mv -f /tmp/temp /tmp/plugdev/deviceinfo
			sed "/^${DEVNAME##*/}/d" "/tmp/plugdev/prodinfo" > /tmp/temp
			eventprod=`cat /tmp/plugdev/prodinfo | grep "${DEVNAME##*/}" | cut -d' ' -f3- | sed 's/\,//g'` 
			mv -f /tmp/temp /tmp/plugdev/prodinfo
#   		        echo "${DEVNAME##*/}" >> /tmp/plugdev/ejected_devicename
			sh_write_evt evt_plgd_ej sss "${DEVNAME##*/}" Storage "${eventprod}"

			# Adjust Data Buffering Directories
			(sleep 1; /etc/init.d/dbufd setdir) &

		fi
	fi
	 ;;
	esac
fi

# Remove the lock,
trap "rm -f $FILE_LOCK;exit" SIGTERM SIGINT EXIT SIGQUIT SIGHUP 
exit 0
