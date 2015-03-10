#!/bin/sh

keep=0;

if [ -z "${BUILDDIR}" -o -z "${BUILD}" -o -z "${PROJDIR}" ] ; then
  echo "*"
  echo "** ERROR ** Variables 'BUILD, BUILDDIR and PROJDIR' must be defined."
  echo "*"
  exit 1
fi

if [ "x${1}" = "x-k" ] ; then
  keep=1
  shift
fi

fname=$(basename $1)
fdir=$(cd $(dirname $1 2>/dev/null);pwd)

cd ${BUILDDIR} || exit 2

TARBALNAME=${fdir}/${fname}

if [ -z "${TARBALNAME}" ] ; then
  echo "*"
  echo "* * ERROR * * tarbal name must be given"
  echo "*"
  exit 1
fi

if [ ! -f ${TARBALNAME} ] ; then
  echo "*"
  echo "* * ERROR * * Invalid tarbal name was given"
  echo "*"
  exit 1
fi

DSTART="$(date)"
echo "Checking tarbal..."
BASE_REPLACE_TEXT="/$(basename ${PROJDIR})/${BUILD}/"

tar -xzf ${TARBALNAME} tmp/replace_text

if [ ! -f tmp/replace_text ] ; then
  echo "*"
  echo "* * ERROR * * Invalid tarbal, it seems not to have an OE stage image"
  echo "*"
  exit 1
fi

if ! grep -q "${BASE_REPLACE_TEXT}" tmp/replace_text ; then
  echo "*"
  echo "* * ERROR * * Invalid tarbal, it seems not to be for this appliance"
  echo "*"
  exit 1
fi

if [ ${keep} -eq 0 ] ; then
  echo "removing tmp"
  if [ -d tmp ] ; then
    if [ -d tmp/cache ] ; then
      mv tmp/cache .
    fi
    sudo rm -rf tmp
    if [ -d cache ] ; then
      mkdir tmp
      mv cache tmp/
    fi
  fi
fi

echo "unpacking ${TARBALNAME}"
tar -xzf ${TARBALNAME}

REPLACE_TEXT_WITH="${BUILDDIR}/tmp"
REPLACE_TEXT=$(cat tmp/replace_text)

for f in $(cat tmp/replace_files)
do
  echo "Patching file: ${f}"
  cp ${f} ${f}.tmp
  sed "s:${REPLACE_TEXT}:${REPLACE_TEXT_WITH}:g" ${f}.tmp >${f}
  rm -f ${f}.tmp;
done

BUILD_PKG_LIST="quilt-native pkgconfig-native m4-native libtool-native gnu-config-native automake-native autoconf-native ipkg-native ipkg-utils-native fakeroot-native cacpd-native llconf-native flex-native"

echo "Patching file: build.rc"
chmod 666 build.rc
cp build.rc build.rc.old
sed "s:export INHIBIT_PREBUILT_PACKAGES=\".*\".*$:export INHIBIT_PREBUILT_PACKAGES=\"${BUILD_PKG_LIST}\":" build.rc.old > build.rc

echo "Fixing symbolic link files"
while read l f
do
  echo "link ${l} -> ${REPLACE_TEXT_WITH}/${f}"
  rm -f ${l}
  ln -sf ${REPLACE_TEXT_WITH}/${f} ${l}
  if [ $? -ne 0 ] ; then
    exit 1
  fi
done  << -EOF
$(< tmp/link_files)
-EOF

if [ ${keep} -eq 0 ] ; then
  bitbake ${BUILD_PKG_LIST}
fi

echo "Start  : ${DSTART}"
echo "Finish : $(date)"
