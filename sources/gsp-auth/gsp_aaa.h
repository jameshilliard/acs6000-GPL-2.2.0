/*****************************************************************
 * File: gsp_aaa.h
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_aaa.h
 *
 ****************************************************************/

#ifndef GSP_AAA_H
#define GSP_AAA_H 1

#include <cyc.h> /* for libcyc_list */

/* The resource structure is defined this way so that it leaves
 * room for new types that we may add in the future */
enum resource_type {
	GSP_ACL_RES_INVALID = -1,
	GSP_ACL_RES_NODE = 0,
};

#define GSP_NODE_PATH_SZ 128
struct gsp_acl_resource {
	enum resource_type type;
	union {
		char node_path[GSP_NODE_PATH_SZ]; /* Node path string */
	} res;
};

enum perm_category {
	GSP_ACL_CATEGORY_UNKNOWN = 0,
	GSP_ACL_CATEGORY_CONFIG,
	GSP_ACL_CATEGORY_MAINT,
	GSP_ACL_CATEGORY_MONITORING,
	GSP_ACL_CATEGORY_SECURITY,
	GSP_ACL_CATEGORY_ACCESS,
};

struct gsp_acl_permission {
	enum perm_category category;
	int perm;     /* bit map of GSP_PERM_* associated with
		       * category; see gsp_acl.h */
};

struct gsp_acl_item {
	struct gsp_acl_resource res;
	struct gsp_acl_permission perm;
};

struct gsp_acl_session {
	/* These linked lists should hold elements of type
	 * struct gsp_acl_item */
	libcyc_list *auth_acls; // ACLs from authentication (e.g. pam-tacplus)
	libcyc_list *cfg_acls;  // ACLs from configuration
};

#define ADSAP2_METHOD "adsap2"
#define GSP_ADSAP2_CONN_DELIM '.'

#endif /* GSP_AAA_H */
