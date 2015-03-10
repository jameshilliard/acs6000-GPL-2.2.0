#set AIDP model (cat /dev/outlet to open the driver)
/sbin/udevsettle
cat /dev/outlet 2>/dev/null
/bin/init_aidp.sh
