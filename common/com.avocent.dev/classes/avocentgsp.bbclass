inherit base

#
#
#The following line need discuss
#FILES_${PN} = "${libdir}/*.so"

avocentgsp_do_compile () {
    echo "Avocent overwritten compile ..."
    oe_runmake all
}

avocentgsp_do_stage () {
    echo "Avocent overwritten stage ..."

    for fh in ${EXPORT_H}
    do
	FN=$(echo $fh | sed -r 's,:.*,,')
	SUBDIR=$(echo $fh | sed -r 's,.*:,,')
	if [[ "$SUBDIR" == "$FN" ]] 
		then SUBDIR="" 
	fi
        mkdir -p ${STAGING_INCDIR}/$SUBDIR
        cp --dereference -R $FN ${STAGING_INCDIR}/$SUBDIR
    done

    for fla in ${EXPORT_LIBA}
    do
        FN=$(basename $fla| sed -r 's,\.a.*,,')
        DIR=$(dirname $fla)
        oe_libinstall -C $DIR -a $FN ${STAGING_LIBDIR}
    done

    for flso in ${EXPORT_LIBSO}
    do
        FN=$(basename $flso| sed -r 's,\.so.*,,')
        DIR=$(dirname $flso)
        oe_libinstall -C $DIR -so $FN ${STAGING_LIBDIR}
    done	
}

avocentgsp_do_install () {
    echo "Avocent overwritten install ..."
    for fb in ${INSTALL_FILES}
    do
	FN=$(echo $fb | awk -F: '{print $1}')
	SUBDIR=$(echo $fb | awk -F: '{print $2}')
	INSTMOD=$(echo $fb | awk -F: '{print $3}')
	if [ "" = "$INSTMOD" ]; then
    		# default install mod os 0644
		INSTMOD=0644
	fi
	if [[ "$SUBDIR" == "$FN" ]] 
		then SUBDIR="" 
	fi
        install -d ${D}$SUBDIR
        install -m ${INSTMOD} $FN ${D}$SUBDIR
    done
}


EXPORT_FUNCTIONS do_compile do_stage do_install
