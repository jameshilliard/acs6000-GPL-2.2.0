#!/bin/sh
AUTH_FILE=/etc/appliance.ini
PAMD=/etc/pam.d
PAM_WEB=web
PAM_LOGIN=login
PAM_SSHD=sshd
PAM_PPP=ppp
PAM_IPPP=ippp

VARS=$(awk '
    BEGIN {
      FS="[ \t=]+"
      SSIGNON=""
      SSIGNON_T=""
      PPPD_AUTH=""
      UNIT_AUTH=""
      STATE=0
    }
    $0 ~ /^$/ { next }
    #{ print "P1=" $1 " P2=" $2 " P3=" $3 }
    $1 == "[auth]" {
      STATE=1
      next
    }
    $2 == "type" {
      if ( STATE == 1 ) {
        UNIT_AUTH=$3
        next
      }
    }
    $2 == "type-ppp" {
      if ( STATE == 1 ) {
        PPPD_AUTH=$3
        next
      }
    }
    $2 == "single-sign-on" {
      if ( STATE == 1 ) {
        SSIGNON=$3
        next
      }
    }
    $2 == "single-sign-on-type" {
      if ( STATE == 1 ) {
        SSIGNON_T=$3
        next
      }
    }
    $1 ~ /^\[.*\]/ {
      #print $0
      if ( STATE == 1 ) {
        STATE = 0
        next
      }
    }
  END {
    if ( SSIGNON == "yes" && SSIGNON_T != "none" && SSIGNON_T != "") {
      UNIT_AUTH=SSIGNON_T
    }
    if ( PPPD_AUTH != "" ) {
      printf "PPPD_AUTH=%s ", PPPD_AUTH
    }
    if ( UNIT_AUTH != "" ) {
      printf "UNIT_AUTH=%s", UNIT_AUTH
    }
  }
  ' ${AUTH_FILE})

if [ -n "${VARS}" ] ; then
  export ${VARS}
fi

if [ -z "${UNIT_AUTH}" ]; then
  UNIT_AUTH=local
fi

if [ -z "${PPPD_AUTH}" ]; then
  PPPD_AUTH=local
fi

cd ${PAMD} || exit

if [ ! -f ${UNIT_AUTH} ]; then
  echo "Error: can not find pam module '${PAMD}/${UNIT_AUTH}' for unit"
  if [ "${UNIT_AUTH}" != "local" ] ; then
    UNIT_AUTH=local
    echo "Trying module '${PAMD}/${UNIT_AUTH}'"
  fi
fi

if [ ! -f ${PPPD_AUTH} ]; then
  echo "Error: can not find pam module '${PAMD}/${PPPD_AUTH}' for ppp"
  if [ "${PPPD_AUTH}" != "local" ] ; then
    PPPD_AUTH=local
    echo "Trying module '${PAMD}/${PPPD_AUTH}'"
  fi
fi

rm -f ${PAM_WEB} ${PAM_LOGIN} ${PAM_SSHD} ${PAM_PPP} ${PAM_IPPP}

ln -s ${UNIT_AUTH} ${PAM_WEB}
ln -s ${UNIT_AUTH} ${PAM_LOGIN}
ln -s ${UNIT_AUTH} ${PAM_SSHD}
ln -s ${PPPD_AUTH} ${PAM_IPPP}
ln -s ${PPPD_AUTH} ${PAM_PPP}
