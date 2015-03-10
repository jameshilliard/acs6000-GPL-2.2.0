#!/bin/bash

[ "x$UID" = "x0" ] || exit 1

if [ -e /var/caesar_app.tar.gz ]; then
	pushd /tmp > /dev/null
	tar zxf /var/caesar_app.tar.gz

	if  [ $? = 0 ]; then
		cd linux-2.6-intel
		./caesar.install now > /var/log/caesar_app.log 2>&1
	else
		popd > /dev/null
		echo "oops, the Caesar installation package is broken!!!!" 
		exit 1
	fi
	popd > /dev/null

	rm -rf /tmp/linux-2.6-intel

	rm -f /var/caesar_app.tar.gz
	#mv -f /var/caesar_app.tar.gz /var/caesar_app.tar.gz.bak
fi

