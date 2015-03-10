#!/bin/bash

data_dir="/data"

if [ -x /bin/realroot.sh ]; then
	. /bin/realroot.sh
else
	echo "Can not detect root!!!"
	export real_root="/dev/sda2"
fi

device=`echo $real_root | sed -rn "s/\/dev\/([sh]d[a-z])([0-9]+)/\1/p"`
part_num=`echo $real_root | sed -rn "s/\/dev\/([sh]d[a-z])([0-9]+)/\2/p"`
data_part_num=`expr $part_num + 3`

data_dev="/dev/$device$data_part_num"

if [ -x /usr/sbin/restore_data.sh ]; then
	/usr/sbin/restore_data.sh 
fi

[ -d $data_dir ] || mkdir -p $data_dir
mount -t ext3 $data_dev $data_dir
if [ -x /sbin/cslnk.sh ]; then
	/sbin/cslnk.sh
fi

#mount --bind $data_dir/home /home
#mount --bind $data_dir/home/root /root
