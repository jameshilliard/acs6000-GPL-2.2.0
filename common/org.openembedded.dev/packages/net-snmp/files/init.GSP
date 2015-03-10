#! /bin/sh
# /etc/init.d/snmpd: start snmp daemon.

test -x /usr/sbin/snmpd || exit 0

# Defaults
export MIBDIRS=/usr/share/snmp/mibs
SNMPDRUN=yes
IPV6=$(llconf ini -i /etc/appliance.ini -s get appliance/ipv6-support)
if [ "x$IPV6" == "xyes" ]; then
    SNMPDOPTS='-Lsd -Lf /dev/null -p /var/run/snmpd.pid udp:161,udp6:161'
else
    SNMPDOPTS='-Lsd -Lf /dev/null -p /var/run/snmpd.pid'
fi

# Cd to / before starting any daemons.
cd /

case "$1" in
  start)
    echo -n "Starting network management services:"
    if [ "$SNMPDRUN" = "yes" -a -f /etc/snmp/snmpd.conf ]; then
    start-stop-daemon -S -x /usr/sbin/snmpd \
        -- $SNMPDOPTS
    echo -n " snmpd"
    fi
    echo "."
    ;;
  stop)
    echo -n "Stopping network management services:"
    start-stop-daemon -K -x /usr/sbin/snmpd
    rm -f /var/net-snmp/snmpd.conf
    echo -n " snmpd"
    echo "."
    ;;
  restart|reload|force-reload)
    echo -n "Restarting network management services:"
    start-stop-daemon -K -x /usr/sbin/snmpd > /dev/null
    # Allow the daemons time to exit completely.
    RET=1
    while [ $RET -ne 0 ]; do
        sleep 2
        rm -f /var/net-snmp/snmpd.conf
        if [ "$SNMPDRUN" = "yes" -a -f /etc/snmp/snmpd.conf ]; then
            start-stop-daemon -S -x /usr/sbin/snmpd -- $SNMPDOPTS > /dev/null
            RET=$?
            if [ $RET -eq 0 ]; then
                echo -n " snmpd"
            else
                echo -n "."
            fi
        fi
    done
    echo "."
    ;;
  *)
    echo "Usage: /etc/init.d/snmpd {start|stop|restart|reload|force-reload}"
    exit 1
esac

exit 0
