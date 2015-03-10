
BDINFO_DIR=/proc/sys/avocent/bdinfo
HOST_FILE=/etc/hostname
ACS_PREFIX=ACS60

if test ! -f ${HOST_FILE} ; then
  if [ $(cat ${BDINFO_DIR}/family) -eq 38 ] ; then
    if [ $(cat ${BDINFO_DIR}/ports) -lt 10 ] ; then
      ACS_PREFIX=ACS600
    fi
    HOSTNAME=$ACS_PREFIX$(cat ${BDINFO_DIR}/ports)-$(cat ${BDINFO_DIR}/serial\#)
    echo "Hostname set to '${HOSTNAME}'"
    echo ${HOSTNAME} > ${HOST_FILE}
  fi
fi

if test -f ${HOST_FILE}
then
  echo "Hostname: '$(cat ${HOST_FILE})'"
  hostname -F ${HOST_FILE}
fi
