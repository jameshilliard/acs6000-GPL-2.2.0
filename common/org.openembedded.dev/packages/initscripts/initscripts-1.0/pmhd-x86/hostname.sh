
BDINFO_DIR=/etc/sys/avocent/bdinfo
BOARD_DIR=/etc/sys/avocent/board
HOST_FILE=/etc/hostname
HOSTNAME=Avocent-PM

if [ ! -f ${HOST_FILE} -o ! -s ${HOST_FILE} ] ; then
  if [ $(cat ${BDINFO_DIR}/family) -eq 43 ] ; then
    HOSTNAME=$(sed "s/\(PM[1-9]\)0\(.*\)/\1000-\2/" ${BOARD_DIR}/model)-$(cat ${BDINFO_DIR}/serial)
    echo "Hostname set to '${HOSTNAME}'"
    echo ${HOSTNAME} > ${HOST_FILE}
  elif [ $(cat ${BDINFO_DIR}/family) -eq 38 ] ; then
    HOSTNAME=Avocent-PM-$(cat ${BDINFO_DIR}/serial\#)
    echo "Hostname set to '${HOSTNAME}'"
    echo ${HOSTNAME} > ${HOST_FILE}
  fi
fi

if test -f ${HOST_FILE}
then
  echo "Hostname: '$(cat ${HOST_FILE})'"
  hostname -F ${HOST_FILE}
fi

