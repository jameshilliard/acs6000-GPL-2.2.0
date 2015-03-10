DESCRIPTION = "mergepont5200"
HOMEPAGE = "http://www.avocent.com"
SECTION = "caesar"
SRC_URI = "svn://172.26.24.4/oepackages/trunk/;module=mergepoint5200;rev=10 \
	file://cross-compile.patch;patch=1 \
	"

#DEPENDS="openssl unixodbc tcl postgresql"
DEPENDS = "dsi5200-image"

S="${WORKDIR}/mergepoint5200/trunk"

#inherit  native

EXTRA_OEMAKE = "-f bldconf.lnx"

python do_unpack() {
	bb.build.exec_func('base_do_unpack', d)
	bb.build.exec_func('svn_do_unpack', d)
}

svn_do_unpack () {
	oenote "S ${S}"
	(cd ${S}; tar zxvf mergepoint5200-3.0.0.tar.gz)
}

#do_patch() {
#	pushd /opt/os/work/mergepoint5200-1.0-r0/mergepoint5200/trunk
#	tar zxvf mergepoint5200-3.0.0.tar.gz
#	patch -d builds -p1 < ../patches/mergepoint5200.patch
#	popd
#}

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
	
	oenote "PATH $PATH"
   	cd builds;./go.py
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

