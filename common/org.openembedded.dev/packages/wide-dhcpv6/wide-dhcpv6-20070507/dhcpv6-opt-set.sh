#!/bin/sh

dhcpv6_opt_start () {
	source /etc/dhcpv6-opt.conf

	if [ "x$DNS" == "xno" -a "x$DOMAIN" == "xno" ]; then
		exit 0
	fi

	unset DHCPINTF

	for itf in $(llconf ifupdown -i /etc/network/interfaces -s list iface); do
		for i in $(llconf ifupdown -i /etc/network/interfaces -s get iface/$itf/method); do
			if [ "x$i" == "xdhcpv6" ]; then
				DHCPINTF=$itf
			fi
		done
	done
	
	if [ -z "$DHCPINTF" ]; then
		for itf in $(llconf ifupdown -i /etc/network/interfaces -s list auto); do
			RES=$(echo $itf | grep eth)
			if [ $? -eq 0 ]; then
				dhcp6c -c /etc/dhcp6c-opt.conf -p /var/run/dhcp6c-opt-$itf.pid $itf
			fi
		done
	fi
}

dhcpv6_opt_stop () {
OPTSET=/var/run/dhcpv6-opt.set

	for i in $(ls /var/run/dhcp6c-opt-*.pid 2>/dev/null); do
		kill $(cat $i)
		rm $i
	done
	if [ -f $OPTSET ]; then
		cat $OPTSET | while read LINE; do
			FILE=$(echo $LINE | cut -d':' -f1)
			CONF=$(echo $LINE | cut -d':' -f2)
			sed -i "/$CONF/d" $FILE
		done
		rm $OPTSET
	fi
}

case "$1" in
	start)
		dhcpv6_opt_start
	;;
	*)
		dhcpv6_opt_stop
	;;
esac
