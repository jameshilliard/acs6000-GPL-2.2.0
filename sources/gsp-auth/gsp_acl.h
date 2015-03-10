/*****************************************************************
 * File: gsp_acl.h
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_acl.h
 *
 ****************************************************************/

#ifndef GSP_ACL_H
#define GSP_ACL_H 1

#include <gsp_aaa.h>

/****************************************************************
 * NOTE: Below is the set of GSP Permissions. When a Permission 
 * is added here, it should be mapped to a DSView access right in 
 * gsl_acl_adsap2.cc. If it IS not, then DSView users will never
 * have the new permissions, which is probably NOT what you want.
 *
 * Contact eduardo.fernandez@avocent.com for further information
 *   about this need.
 ****************************************************************/

/* To be used with CATEGORY_CONFIG */
#define GSP_PERM_CFG_CONFIG ~0
#define GSP_PERM_CFG_NONE 0
#define GSP_PERM_CFG_RO 1<<0
#define GSP_PERM_CFG_RW (1<<1 | GSP_PERM_CFG_RO)

/* To be used with CATEGORY_MAINT */
#define GSP_PERM_MAINT_MAINT ~0
#define GSP_PERM_MAINT_NONE 0
#define GSP_PERM_MAINT_BACKUP 1<<0
#define GSP_PERM_MAINT_RESTORE 1<<1
#define GSP_PERM_MAINT_REBOOT 1<<2
#define GSP_PERM_MAINT_FW_UPGRADE 1<<3
#define GSP_PERM_MAINT_RESTART_DAEMON 1<<4
#define GSP_PERM_MAINT_STOP_DAEMON 1<<5
#define GSP_PERM_MAINT_CARDBUS_CTL 1<<6
#define GSP_PERM_MAINT_NETWORK_RO 1<<7
#define GSP_PERM_MAINT_NETWORK_RW (1<<8 | GSP_PERM_MAINT_NETWORK_RO)
#define GSP_PERM_MAINT_TRANSF_FILES 1<<9

/* To be used with CATEGORY_MONITORING */
#define GSP_PERM_MONITORING_MONITOR ~0
#define GSP_PERM_MONITORING_NONE 0
#define GSP_PERM_MONITORING_SESSION_VIEW 1<<0
#define GSP_PERM_MONITORING_SESSION_KILL 1<<1
#define GSP_PERM_MONITORING_PORT_STATUS 1<<2
#define GSP_PERM_MONITORING_SYS_INFO 1<<3
#define GSP_PERM_MONITORING_ALARMS_VIEW 1<<4
#define GSP_PERM_MONITORING_DATALOG_VIEW 1<<5

/* To be used with CATEGORY_SECURITY */
#define GSP_PERM_SECURITY_SECURITY ~0
#define GSP_PERM_SECURITY_NONE 0
#define GSP_PERM_SECURITY_PASSWD_CHNG 1<<0
#define GSP_PERM_SECURITY_PASSWD_CHNG_OWN 1<<1
#define GSP_PERM_SECURITY_USER_ADD 1<<2
#define GSP_PERM_SECURITY_USER_DEL 1<<3
#define GSP_PERM_SECURITY_GROUP_ADD 1<<4
#define GSP_PERM_SECURITY_GROUP_DEL 1<<5
#define GSP_PERM_SECURITY_AUTHORIZATION 1<<6
#define GSP_PERM_SECURITY_PW_RULES 1<<7
#define GSP_PERM_SECURITY_INSTALL_CERT 1<<8

/* To be used with CATEGORY_ACCESS */
#define GSP_PERM_ACCESS_SHELL 1<<0
#define GSP_PERM_ACCESS_NONE 0
#define GSP_PERM_ACCESS_SESSION_RO 1<<1
#define GSP_PERM_ACCESS_SESSION_RW (1<<2 | GSP_PERM_ACCESS_SESSION_RO)
#define GSP_PERM_ACCESS_POWER 1<<3
#define GSP_PERM_ACCESS_MULTI_SESS_MENU 1<<4
#define GSP_PERM_ACCESS_MULTI_SESS_RO 1<<5
#define GSP_PERM_ACCESS_MULTI_SESS_RW (1<<6 | GSP_PERM_ACCESS_MULTI_SESS_RO)
#define GSP_PERM_ACCESS_MULTI_SESS_KILL 1<<7
#define GSP_PERM_ACCESS_MULTI_SESS_SENDMSG 1<<8
#define GSP_PERM_ACCESS_SESSION_POWER 1<<9
#define GSP_PERM_ACCESS_POWER_OUTLET 1<<10
#define GSP_PERM_ACCESS_DATABUFFER_MGMT 1<<11
#define GSP_PERM_ACCESS_LAUNCH_VIEWER 1<<12
#define GSP_PERM_ACCESS_SESSION_VIRTUAL_MEDIA 1<<13
#define GSP_PERM_ACCESS_SESSION_VIRTUAL_MEDIA_RESERVED 1<<14
#define GSP_PERM_ACCESS_POWER_OUTLET_ANY 1<<15
#define GSP_PERM_ACCESS_POWER_INFO 1<<16

enum acl_status {
	GSP_ACL_STAT_ERR = -1,
	GSP_ACL_STAT_UNKNOWN = 0,
	GSP_ACL_STAT_AUTH,     /* Authorized */
	GSP_ACL_STAT_NOAUTH,   /* Not authorized */
};

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Public C GSP ACL API for applications
 */

enum acl_status gsp_acl_authorize(struct sid_data *,
				  struct gsp_acl_permission *,
				  struct gsp_acl_resource *);

/* End public C GSP ACL API for applications */

#ifdef __cplusplus
}
#endif

#endif /* GSP_ACL_H */
