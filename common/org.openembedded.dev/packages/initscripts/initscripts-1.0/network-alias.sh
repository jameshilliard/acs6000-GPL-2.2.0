#!/bin/sh

#
# Define network aliases for serial ports
#

PGM=$(basename ${0})
FILE_LOCK=/var/run/${PGM}.pid
CONFIG=port-ipadd
OLD_CNF=/var/run/${CONFIG}
NEW_CNF=/etc/${CONFIG}
TMP_CNF=/tmp/${CONFIG}

TCPPORTS=tcpport-alias
OLD_TCPPORTS=/var/run/${TCPPORTS}
NEW_TCPPORTS=/etc/${TCPPORTS}

#
# p1: port
# p2: ipv4 address
# p3: interface
#
add_ipv4()
{
  if [ ${2} = - -o  ${3} = - ] ; then
    return
  fi
  ifconfig ${3}:${1} ${2} netmask 255.255.255.255 up
}
   
#
# p1: port
# p2: ipv4 address
# p3: interface
#
del_ipv4()
{
  if [ ${2} = - -o  ${3} = - ] ; then
    return
  fi
  ifconfig ${3}:${1} ${2} down
}
   
#
# p1: ipv6 address
# p2: interface
#
add_ipv6()
{
  if [ ${1} = - -o  ${2} = - ] ; then
    return
  fi
  ifconfig ${2} inet6 add ${1}/128
}

#
# p1: ipv6 address
# p2: interface
#
del_ipv6()
{
  if [ ${1} = - -o  ${2} = - ] ; then
    return
  fi
  ifconfig ${2} inet6 del ${1}/128
}

while [ -f ${FILE_LOCK} ]
do
  kill -0 $(cat ${FILE_LOCK})
  if [ $? -ne 0 ]; then
    break;
  fi
  sleep 1
done

echo $$ > ${FILE_LOCK}

>${TMP_CNF}

if [ -f ${NEW_CNF} ]; then
  sed 's/^[1-9][0-9]*/& wen/' <${NEW_CNF} >>${TMP_CNF}
fi

if [ -f ${OLD_CNF} ]; then
  sed 's/^[1-9][0-9]*/& dlo/' <${OLD_CNF} >>${TMP_CNF}
fi

old=0

sort -n ${TMP_CNF} | while read port ori ipv4_ad ipv4_if ipv6_ad ipv6_if
do
  [ "${port:0:1}" \< "1" -o "${port:0:1}" \> "9" ] && continue

  if [ ${ori} = dlo ] ; then
    if [ ${old} -eq 1 ] ; then
      # $o_port is not present in new configuration
      del_ipv4 ${o_port} ${o_ipv4_ad} ${o_ipv4_if}
      del_ipv6 ${o_ipv6_ad} ${o_ipv6_if}
    fi
    old=1
    # save current old port information
    o_port=${port}
    o_ipv4_ad=${ipv4_ad}
    o_ipv4_if=${ipv4_if}
    o_ipv6_ad=${ipv6_ad}
    o_ipv6_if=${ipv6_if}
    continue
  fi

  if [ ${old} -eq 0 ] ; then
    # $port was not in old configuration, and there is no previous alias to be deleted
    add_ipv4 ${port} ${ipv4_ad} ${ipv4_if}
    add_ipv6 ${ipv6_ad} ${ipv6_if}
    continue;
  fi

  old=0

  if [ ${o_port} != ${port} ] ; then
    # $port was not in old configuration, but there is a previous alias to be deleted
    del_ipv4 ${o_port} ${o_ipv4_ad} ${o_ipv4_if}
    del_ipv6 ${o_ipv6_ad} ${o_ipv6_if}
    add_ipv4 ${port} ${ipv4_ad} ${ipv4_if}
    add_ipv6 ${ipv6_ad} ${ipv6_if}
    continue
  fi

  # $port was in old configuration: check if aliased interface (IPv4 or IPv6) changed
  if [ ${o_ipv4_ad} != ${ipv4_ad} -o ${o_ipv4_if} != ${ipv4_if} ] ; then
    del_ipv4 ${o_port} ${o_ipv4_ad} ${o_ipv4_if}
    add_ipv4 ${port} ${ipv4_ad} ${ipv4_if}
  fi
  if [ ${o_ipv6_ad} != ${ipv6_ad} -o ${o_ipv6_if} != ${ipv6_if} ] ; then
    del_ipv6 ${o_ipv6_ad} ${o_ipv6_if}
    add_ipv6 ${ipv6_ad} ${ipv6_if}
  fi

done

if [ ${old} -eq 1 ] ; then
  del_ipv4 ${o_port} ${o_ipv4_ad} ${o_ipv4_if}
  del_ipv6 ${o_ipv6_ad} ${o_ipv6_if}
fi

rm -f ${TMP_CNF}
cp ${NEW_CNF} ${OLD_CNF}

# this is to guarantee the first run of tcp_port_xinetd.sh will be ok
cp ${NEW_TCPPORTS} ${OLD_TCPPORTS}

rm -f  ${FILE_LOCK}
