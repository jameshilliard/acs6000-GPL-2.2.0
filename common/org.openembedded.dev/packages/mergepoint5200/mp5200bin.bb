DESCRIPTION = "mergepont5200 bin"
HOMEPAGE = "http://www.avocent.com"
SECTION = "app"

#DEPENDS="openssl unixodbc tcl postgresql"
DEPENDS = "dsi5200_cvs"

S="${WORKDIR}/../mergepoint5200-1.0-r0/project_caesar"

#EXTRA_OEMAKE = "-f bldconf.lnx"
#
PARALLEL_MAKE=""

export CC="${HOST_PREFIX}gcc -isystem ${STAGING_INCDIR}"

#python do_unpack() {
#	bb.build.exec_func('base_do_unpack', d)
#	bb.build.exec_func('cvs_do_unpack', d)
#}

#cvs_do_unpack () {
#	if [ -z ${MERGEPOINT_TAG} ]; then
#		(cd ${S}/modules; ./caesarco.py  -r HEAD all;)
#	else
#		(cd ${S}/modules; ./caesarco.py  -r ${MERGEPOINT_TAG} all;)
#	fi
#}

#do_compile() { 
#	[ -d bin ] || mkdir bin
#	for cmd in `ls ${CROSS_DIR}/bin/${HOST_PREFIX}*`; do
#		cmdalias=`echo $cmd | sed -r "s/.*${HOST_PREFIX}//g"`
#		alias $cmdalias=${HOST_PREFIX}$cmdalias
#		ln -sf $cmd bin/$cmdalias
#	done
#
#	rm -f bin/gcc bin/g++
#
#	echo -e "#!/bin/bash\n ${HOST_PREFIX}gcc -isystem ${STAGING_INCDIR}" '$@' > bin/gcc
#	echo -e "#!/bin/bash\n ${HOST_PREFIX}g++ -isystem ${STAGING_INCDIR}" '$@' > bin/g++
#	
#	chmod +x bin/gcc bin/g++
#
#	export JAVA_HOME=/opt/j2sdk1.4.2_09
#	export ANT_HOME=/usr/local
#	
#	export PATH="$JAVA_HOME/bin:${S}/bin:$PATH"
#	
#	false
#   	(cd builds; ./go.py)
#}

do_install () {
	pushd ${S}/builds/Installation_embed
	#appname="caesar_app`date +%Y%m%d%H%M`.tgz" 
     	#./build.sh && tar czf ${appname} linux-2.6-intel
     	#if [ -e ${appname} ]; then
	#	cp -f ${appname} ${DEPLOY_DIR_IMAGE}
	#fi
	
	# To make a upgrade bin file.
	export BINFILE=/opt/MP_3.0.0.2006101301.bin
	export GZFILE=data.tar.gz
	export TARFILE=data.tar
	export EPMFILE=caesar.list
	export INSTALLFILES=install.files
	export IGNOREFILES="./www/htdocs/htmls.tar ./etc/network/interfaces"
	export OUTPUTDIR=../source/common/output/server
	ezport OUTBIN=MergePoint5200_`date +%Y%m%d%H%M`.bin
	[ -e makebin.log ] && rm -f makebin.log
	[ -e install.files ] && rm -f install.files
	[ -e usr/share/zoneinfo ] && chmod u+x usr/share/zoneinfo
	unpackage -x ${BINFILE} -o ${GZFILE} --on on.sh --pre pre_upgrade.sh --post post_upgrade.sh
	chmod u+r ${GZFILE}
	gunzip ${GZFILE}
    	tar xf ${TARFILE}
    	if [ ! -d dsview/DSI5200/data ]; then
        	tar --delete -f ${TARFILE} ./dsview/DSI5200/etc
        	mv dsview/DSI5200/etc dsview/DSI5200/data
        	tar -uf ${TARFILE} ./dsview/DSI5200/data
    	fi
        # All file should to be installed.
    	grep "^f 0" ${EPMFILE} > $INSTALLFILES
    	sed -e 's|${SRCROOT}|/home/caesar/project_caesar/builds/source/common/output/server|' $INSTALLFILES \
    	| sed -e 's| ./| /home/caesar/project_caesar/builds/Installation_embed/|' \
    	| sed -e 's| ../| /home/caesar/project_caesar/builds/|' > tmp.list
    	mv tmp.list $INSTALLFILES

	# Prepare work, to make local files' mode same with dest.
	strip ./dsview/DSI5200/bin/aidpd
	strip ${OUTPUTDIR}/aidpd
	chmod +s ${OUTPUTDIR}/*.cgi
	chmod +s ${OUTPUTDIR}/clp

    	while read LINE
    	do
        	mod=`echo $LINE | cut -d " " -f2`
        	installpath=.`echo $LINE | cut -d " " -f5`
        	installdir=.`dirname $installpath`
        	srcpath=`echo $LINE | cut -d " " -f6`
        	rootdir=`echo $installpath | cut -c1-6`
        	if [ $rootdir = "./data" ]; then
            		continue
        	fi
        	echo $IGNOREFILES | grep $installpath
        	if [ 0 -eq $? ]; then
            		continue
        	fi
        	# new added file
        	if [ ! -f $installpath ]; then
            		mkdir -p $installdir
        	else
            		diff -q $installpath $srcpath
        	fi
        	cp -f $srcpath $installpath
        	tar -uf $TARFILE $installpath
    	done < $INSTALLFILES

	tar --delete -f ${TARFILE} ./var/www/htdocs
	cp ${OUTPUTWEB}/htmls.tar var/www/htdocs
	pushd var/www/htdocs
	tar xf htmls.tar
	popd

	gzip ${TARFILE}

	package -o ${OUTBIN} -t CAESARUPGRADEBIN --pre /home/caesar/appliance/upgrade/pre_upgrade.sh --on /home/caesar/appliance/upgrade/on.sh --post /home/caesar/appliance/upgrade/post_upgrade.sh CAESAR_V_1.0.0.0 -i ${GZFILE}
    	chmod a+r ${OUTBIN}

	popd
} 

