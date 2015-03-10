
BDINFO_DIR=/proc/sys/avocent/bdinfo
HOST_FILE=/etc/hostname
PREFIX=FW-S10

if test ! -f ${HOST_FILE} ; then
  if [ $(cat ${BDINFO_DIR}/family) -eq 38 ] ; then
    if [ $(cat ${BDINFO_DIR}/ports) -lt 10 ] ; then
      PREFIX=FW-S100
    fi
    MAC=`cat ${BDINFO_DIR}/mac1|cut -d ':' -f 4-|sed "s/:/-/g"`
    HOSTNAME=FW-S10$(cat ${BDINFO_DIR}/ports)SR-${MAC}
    echo "Hostname set to '${HOSTNAME}'"
    echo ${HOSTNAME} > ${HOST_FILE}
  fi
fi

if test -f ${HOST_FILE}
then
  echo "Hostname: '$(cat ${HOST_FILE})'"
  hostname -F ${HOST_FILE}
fi

