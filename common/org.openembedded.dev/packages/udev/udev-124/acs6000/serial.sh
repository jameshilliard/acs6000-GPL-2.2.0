#!/bin/sh

MAX=$(expr ${1} - 65)

if [ ${MAX} -eq 0 ] ; then
  echo "ttyM1"
  mknod -m 660 /dev/ttyA1 c ${2} ${1}
  chown root:tty /dev/ttyA1
  exit 0
fi

if [ ${MAX} -gt $(/bin/cat /proc/sys/avocent/bdinfo/ports) ] ; then
  M=$(expr ${MAX} + 1)
else
  M=${MAX}
fi

echo "ttyS${M}"

