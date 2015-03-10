#!/bin/sh

DNSCONF=/etc/resolv.conf
OPTSET=/var/run/dhcpv6-opt.set

source /etc/dhcpv6-opt.conf

if [ "x$DOMAIN" == "xyes" ]; then
	if [ -n "$new_domain_name" ]; then
		grep $new_domain_name $DNSCONF > /dev/null 2>&1
		if [ $? -ne 0 ]; then
			mv $DNSCONF /tmp/$DNSCONF
			echo "domain $new_domain_name" >> $DNSCONF
			cat /tmp/$DNSCONF >> $DNSCONF
			rm /tmp/$DNSCONF
			echo "$DNSCONF:domain $new_domain_name" >> $OPTSET
		fi 
	fi
fi
if [ "x$DNS" == "xyes" ]; then
	if [ -n "$new_domain_name_servers" ]; then
		grep $new_domain_name_servers $DNSCONF > /dev/null 2>&1
		if [ $? -ne 0 ]; then 
			mv $DNSCONF /tmp/$DNSCONF
			echo "nameserver $new_domain_name_servers" >> $DNSCONF
			cat /tmp/$DNSCONF >> $DNSCONF
			rm /tmp/$DNSCONF
			echo "$DNSCONF:nameserver $new_domain_name_servers" >> $OPTSET
		fi
	fi
fi
