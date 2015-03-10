
HOST_FILE=/etc/hostname

if test ! -f ${HOST_FILE} ; then
  HOSTNAME="ACS6000-VM"
  echo "Hostname set to '${HOSTNAME}'"
  echo ${HOSTNAME} > ${HOST_FILE}
fi

if test -f ${HOST_FILE}
then
  echo "Hostname: '$(cat ${HOST_FILE})'"
  hostname -F ${HOST_FILE}
fi
