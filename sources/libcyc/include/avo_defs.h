/* -*- linux-c -*- */
/*****************************************************************
 * File: avo_defs.h
 *
 * Copyright (C) 2004-2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: libcyc header
 *      
 ****************************************************************/

#ifndef AVO_DEFS_H
#define AVO_DEFS_H 1

/* Some definitions */
#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif
#ifndef ERROR
#define ERROR -1
#endif

#define LOCK_EXT "pid"
#define SOCK_EXT "sock"

#ifndef MAX_STR_LEN
#define MAX_STR_LEN 255
#endif
#define IP_ADDR_STR_LEN 16
#define SESS_STR_LEN 32
#define KVM_ALIAS_LEN 21
#define MD5_DIGEST_LEN 16
#define MD5SUM_HEXSTR_LEN MD5_DIGEST_LEN*2 + 1
#define OUTLET_STR_LEN 4
#define PHYSPORT_STR_LEN 6
#define MAX_MSG_LEN 1024     /* Max length for IPC messages */
#define BDINFO_STR_LEN 32
#define GSP_USERNAME_LEN 32
#define KVM_USERNAME_LEN 32
#define PROC_BDINFO_FILE "/proc/bdinfo"
#define PROC_UPTIME_FILE "/proc/uptime"
#define PROC_BDINFO_DELIMITER '!'
#define NETCONF_RUNFILE "/tmp/internal_net.running"
#define VCARD_MAC_FILE_PREFIX "/tmp/vcard_mac"
#define NETCONF_RUNFILE_DELIMITER ':'
#define VCARD_TYPE_FILE "/etc/vcard_type"
#define VCARD_TYPE_DELIMITER ':'
#define KVM_CASC_DELIMITER '.'
#define DEV_RANDOM_FILE "/dev/random"

/* AlterPath Product Defines */
#define KVM_BDINFO_STR "KVM"
#define KVMNET_BDINFO_STR "KVMN"
#define ONS_BDINFO_STR "ONS"
#define KVMNETPLUS_BDINFO_STR "KVMP"
/* For all products with fans */
#define AP_MIN_FAN_SPEED 2000
/* For KVM (analog) */
#define KVM_MAX_FANS 1
#define KVM_MAX_IP_MODULES 0
#define KVM_MAX_STATIONS 2
/* For KVM/net */
#define KVMNET_MAX_FANS 2
#define KVMNET_MAX_IP_MODULES 2
#define KVMNET_MAX_STATIONS 2
/* For KVM/netPlus */
#define KVMNET_PLUS_MAX_FANS 3
#define KVMNET_PLUS_MAX_IP_MODULES 4
#define KVMNET_PLUS_MAX_STATIONS 6
#define KVMNET_PLUS_LOCAL_USER_STATIONS 2
/* For OnSite */
#define ONSITE_MAX_FANS 2
#define ONSITE_MAX_IP_MODULES 2
#define ONSITE_V1_MAX_STATIONS 2
#define ONSITE_V2_MAX_STATIONS 3
#define ONSITE_LOCAL_USER_STATIONS 1
/* For ACS (power supply bitmap) */
#define PS_1 0x01
#define PS_2 0x02
#define PS_MASK 0x03
#define ACS_MAX_PS 2
	
/* File locations */
#define KVMIP_LED_DEVFILE "/dev/cy_kvmip_led"
#define APCTL_DEVFILE "/dev/cy_apctl"
#define LED_DEVFILE "/dev/led"
#define WDT_DEVFILE "/dev/wdt"
#define KVMD_RUNFILE "/var/run/kvmd.pid"
#define KVMD_CONFFILE "/etc/kvmd.conf"
#define KVMD_SLAVES_CONFFILE "/etc/kvmd.slaves"
#define CY_WDT_LED_RUNFILE "/var/run/cy_wdt_led.pid"

/* Program locations */
#define KVM_COMMAND "/bin/kvm"
#define RDP_NAT_COMMAND "/usr/bin/rdp_nat.sh"
#define KVMIPCTL_COMMAND "/usr/bin/kvmipctl"
#define MONITOR_COMMAND "/usr/bin/monitor"
#define RDP_CONNTRACK_COMMAND "/usr/bin/rdp_conntrack"
#define PROXY_COMMAND "/usr/bin/proxy"
#define W_COMMAND "/usr/bin/w"
#define FORWARD_SH_COMMAND "/usr/bin/forward.sh"
#define PING_COMMAND "/bin/ping"
#define PM_COMMAND "/bin/pmCommand"
#define TRANSLATE_COMMAND "/usr/bin/translate.sh"

#ifndef ON
#define ON 1
#endif
#ifndef OFF
#define OFF 0
#endif

#endif /* AVO_DEFS_H */
