DESCRIPTION = "mergepont5200"
HOMEPAGE = "http://www.avocent.com"
SECTION = "app"
SRC_URI = "cvs://${OSA_CVS_REPOS};module=project_caesar \
	file://cross-compile.patch;patch=1 \
	file://osasshd.patch;patch=1 \
	file://solite.patch;patch=1 \
	file://shserver2.patch;patch=1 \
	file://targetutil.patch;patch=1 \
	file://telnet.patch;patch=1 \
	"

#DEPENDS="openssl unixodbc tcl postgresql"
DEPENDS = "dsi5200-image"

S="${WORKDIR}/project_caesar"

EXTRA_OEMAKE = "-f bldconf.lnx"

PARALLEL_MAKE=""

export CC="${HOST_PREFIX}gcc -isystem ${STAGING_INCDIR}"

python do_unpack() {
	bb.build.exec_func('base_do_unpack', d)
	bb.build.exec_func('cvs_do_unpack', d)
}

cvs_do_unpack () {
	if [ -z ${MERGEPOINT_TAG} ]; then
		(cd ${S}/modules; ./caesarco.py  -r HEAD all;)
	else
		(cd ${S}/modules; ./caesarco.py  -r ${MERGEPOINT_TAG} all;)
	fi
}

do_compile() { 
	[ -d bin ] || mkdir bin
	for cmd in `ls ${CROSS_DIR}/bin/${HOST_PREFIX}*`; do
		cmdalias=`echo $cmd | sed -r "s/.*${HOST_PREFIX}//g"`
		alias $cmdalias=${HOST_PREFIX}$cmdalias
		ln -sf $cmd bin/$cmdalias
	done

	rm -f bin/gcc bin/g++

	echo -e "#!/bin/bash\n ${HOST_PREFIX}gcc -isystem ${STAGING_INCDIR}" '$@' > bin/gcc
	echo -e "#!/bin/bash\n ${HOST_PREFIX}g++ -isystem ${STAGING_INCDIR}" '$@' > bin/g++
	
	chmod +x bin/gcc bin/g++

	export JAVA_HOME=/opt/j2sdk1.4.2_09
	export ANT_HOME=/usr/local
	
	export PATH="$JAVA_HOME/bin:${S}/bin:$PATH"
	
#	false
   	(cd builds; ./go.py)
}

do_install () {
	pushd ${S}/builds/Installation_embed
	appname="caesar_app`date +%Y%m%d%H%M`.tgz" 
     	./build.sh && tar czf ${appname} linux-2.6-intel
     	if [ -e ${appname} ]; then
		cp -f ${appname} ${DEPLOY_DIR_IMAGE}
	fi
	popd
} 

