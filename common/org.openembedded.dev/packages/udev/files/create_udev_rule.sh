#!/bin/sh

DIR_RULES=/etc/udev/rules.d
PGM=$(basename ${0})
OLDNAME=""
NEWNAME=""
OPTIND=1
DEBUG=0
CLEAN=0
umask 0022
usage()
{
  echo
  echo "Error: ${1}"
  echo
  echo "USAGE: ${PGM} [-d] -o <oldDeviceName> -n <newDeviceName>"
  echo "       ${PGM} [-d] -c -o <oldDeviceName>"
  echo
  exit -1
}

do_grep()
{
  DEV=$(ls -l /dev/${OLDNAME} | awk '{print substr($5, 1, length($5)-1 ) ":" $6}')

  while read file
  do
    if [ "${DEV}" == "$(cat ${file})" ] ; then
      syspath=$(dirname ${file})
      return;
    fi
  done << -EOF
$(find /sys/${1} -name dev -print)
-EOF
}

while getopts ":o:n:cd" op ; do
  case ${op} in
    o)
      OLDNAME=${OPTARG}
      ;;
    n)
      NEWNAME=${OPTARG}
      ;;
    c)
      CLEAN=1
      ;;
    d)
      DEBUG=1
      ;;
    ?)
      usage "Invalid Option: '${op}'."
      ;;
  esac
done

if [ -z "${OLDNAME}" ]; then
  usage "Current Device name must be given."
fi

if [ -z "${NEWNAME}" -a ${CLEAN} -eq 0 ]; then
  usage "New Device name must be given."
fi

if [ ${CLEAN} -eq 1 ] ; then
  [ -x ${DIR_RULES}/_${OLDNAME}.rules ] && rm -f ${DIR_RULES}/_${OLDNAME}.rules
  exit 0
fi

# This works for net devices and others not renamed
syspath=$(find /sys/class/tty/ -name ${OLDNAME})
[ -z "${syspath}" ] && syspath=$(find /sys/block/ -name ${OLDNAME})
[ -z "${syspath}" ] && syspath=$(find /sys/class/net/ -name ${OLDNAME})

if [ -z "${syspath}" -a -c /dev/${OLDNAME} ] ; then
  do_grep devices
fi

if [ -z "${syspath}" -a -b /dev/${OLDNAME} ] ; then
  do_grep block
fi

if [ -z "${syspath}" ]; then
  usage "Invalid device name '${OLDNAME}'."
fi

RULE_BODY=$(udevinfo -a -p ${syspath} | awk  -f /sbin/udev_rules_awk \
  OLDNAME=${OLDNAME} \
  NEWNAME=${NEWNAME} \
  DEBUG=${DEBUG})

if [ $? != 0 ] ; then
  echo
  echo "ERROR: Could not create rule for device '${OLDNAME}'"
  echo
  echo "${RULE_BODY}"
  echo
  exit 1
fi

rm -f ${DIR_RULES}/_${OLDNAME}.rules
echo ${RULE_BODY} > ${DIR_RULES}/_${NEWNAME}.rules
chmod +x ${DIR_RULES}/_${NEWNAME}.rules
