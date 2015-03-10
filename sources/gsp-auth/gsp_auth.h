/*****************************************************************
 * File: gsp_auth.h
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_auth.h
 *
 ****************************************************************/

#ifndef GSP_AUTH_H
#define GSP_AUTH_H 1

#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include <security/pam_appl.h>

#include <cyc.h>

#include <gsp_aaa.h>

//
// duplication from pam
//
#ifndef GSP_SID_SZ
#define GSP_SID_SZ 32
struct sid_data {
   char sid[GSP_SID_SZ];
   int sid_sz;
};
#endif

#define SID_SZ GSP_SID_SZ /* Deprecated */

struct gsp_auth_user_info {
	char *username;
	int username_sz;
};

struct gsp_auth_group_info {
	char *group_name;
	int group_name_sz;
};

struct gsp_sess_ident {
	struct sid_data sdata;
	unsigned int ref;
};

struct gsp_auth_attr {
	int idle_cnt;    /* Idle timeout counter; must be first member */
};

struct gsp_session_data {
	/* Session-specific attributes and data */
	struct gsp_auth_attr attr; /* Must be first member */

	struct sid_data sid;
};

enum gsp_inet_family {
	GSP_AF_UNKNOWN = -1,
	GSP_AF_INET = AF_INET,
	GSP_AF_INET6 = AF_INET6,
};

/* Private attribute flags */
#define GSP_AUTH_ATTR_STRONG_SEC        1<<0
#define GSP_AUTH_ATTR_AUTH_TARGET       1<<1
#define GSP_AUTH_ATTR_OTK               1<<2
#define GSP_AUTH_ATTR_DONT_MONITOR      1<<3
#define GSP_AUTH_ATTR_KILL              1<<4
#define GSP_AUTH_ATTR_KILL_WAIT         1<<5
#define GSP_AUTH_ATTR_TARGET_CONNECTED  1<<6

enum gsp_auth_session_type {
	GSP_AUTH_SESS_UNKNOWN = -1,
	GSP_AUTH_SESS_SSH = 0,
	GSP_AUTH_SESS_TELNET,
	GSP_AUTH_SESS_CONSOLE,
	GSP_AUTH_SESS_LOGIN,
	GSP_AUTH_SESS_HTTP,
	GSP_AUTH_SESS_HTTPS,
	GSP_AUTH_SESS_DSVIEW,
	GSP_AUTH_SESS_RAW,
};

enum gsp_auth_target_session_type {
	GSP_AUTH_TARG_UNKNOWN = -1,
	GSP_AUTH_TARG_SERIAL = 0,
	GSP_AUTH_TARG_KVM,
	GSP_AUTH_TARG_SOL,
	GSP_AUTH_TARG_POWER,
	GSP_AUTH_TARG_WMI,
	GSP_AUTH_TARG_IPMI,
	GSP_AUTH_TARG_CLI,
};

#define GSP_CID_SZ 16                 /* Defined by Interoperability team */
#define GSP_AUTH_TYPE_SZ 50   /* Size of the authentication method string */
#define GSP_KILL_USERNAME_SZ 32     /* Size of user who's killing session */
struct gsp_auth_privattr {
	int flags;                   /* Bitmap of GSP_AUTH_ATTR_* defines */
	char auth_type[GSP_AUTH_TYPE_SZ]; /* Authentication method string
					   * as specified in the
					   * configuration */
	char curr_auth_type[GSP_AUTH_TYPE_SZ]; /* Current authentication
						* method string (e.g. from
						* PAM_SERVICE) */
	pid_t mpid;         /* PID of process who's managing this session */
	int outlet_authorization;
	int serial_authorization;
	int tacacs_level;
	int radius_service_type;
	int idle_to;              /* Per-session idle timeout, in seconds */
	int kill_cnt;       /* Counter used by session management daemon  *
			     * while waiting for session to be killed     */
	time_t sess_ctime;               /* Time when session was created */
	enum gsp_auth_session_type sess_type;             /* Session type */
	enum gsp_auth_target_session_type targ_sess_type;  /* Target type */
	int target_id;       /* ID of the target the user is connected to */
	char cid[GSP_CID_SZ];      /* Char buffer for Core/transaction ID */
	uid_t uid;                                             /* User ID */
	gid_t gid;                                            /* Group ID */
	union {
		struct in_addr ipv4;      /* Definition from netinet/in.h */
		struct in6_addr ipv6;     /* Definition from netinet/in.h */
	} ipaddr;
	enum gsp_inet_family ipaddr_fam;
	char kill_username[GSP_KILL_USERNAME_SZ]; /* Name of user trying  *
						   * to kill the session  */
};

#define GSP_CONNECTION_ID_SZ 32
struct gsp_auth_adsap2_token {
	int handle; /* Used for future Authorizations via ADSAP2 */
	char conn_id[32];                 /* Connection identifier string */
};

struct gsp_session_privdata {
	/* The sid_data definition comes from pam_appl.h */
	struct sid_data sid;                                   /* Our SID */
	struct sid_data parent_sid;                         /* Parent SID */

	/* Username */
	struct gsp_auth_user_info uinfo;

	/* Group information from remote auth server, if supplied */
	struct gsp_auth_group_info ginfo;

	/* Session-specific attributes and data */
	struct gsp_auth_privattr attr;

	/* Data in the session relevant to ADSAP2 */
	struct gsp_auth_adsap2_token adsap2_tok;

	/* ACLs for this session */
	struct gsp_acl_session session_acls; /* Def. from gsp_aaa.h */
};

enum sid_status {
	GSP_AUTH_SID_ERR = -1,
	GSP_AUTH_SID_VALID = 0,
	GSP_AUTH_SID_EINVAL,
	GSP_AUTH_SID_ERANGE,
	GSP_AUTH_SID_NULL,
};

enum gsp_auth_targ_argtype {
	GSP_AUTH_ARGTYPE_UNKNOWN = 0,
	GSP_AUTH_ARGTYPE_NAME,
};

struct gsp_auth_target {
	enum gsp_auth_targ_argtype argtype; /* Specify active union member */
	union {
		char *name;           /* target name */
	} target;
};

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * C GSP Authentication API for applications
 */

/*
 * PAM Authentication API
 */
int gsp_auth_pam_authenticate(const char *, const char *,
			      const struct pam_conv *,
			      pam_handle_t **, struct sid_data *);
int gsp_auth_pam_authenticate_username(const char *, const char *,
				       const char *, const struct pam_conv *,
				       pam_handle_t **, struct sid_data *);
int gsp_auth_pam_end(pam_handle_t **);

char *gsp_auth_pam_get_orig_service(pam_handle_t *);
char *gsp_auth_pam_get_service(pam_handle_t *);
int gsp_auth_pam_get_sid(pam_handle_t *, struct sid_data *);

/*
 * Session API
 */

/* Session create/destroy */
int gsp_auth_session_create(struct sid_data *);
int gsp_auth_subsession_create(struct sid_data *, struct sid_data *);
int gsp_auth_session_create_username(const char *, struct sid_data *);
int gsp_auth_subsession_create_username(const char *, struct sid_data *,
					struct sid_data *);
int gsp_auth_session_create_adsap2(const char *, unsigned int,
				   struct sid_data *);
int gsp_auth_subsession_create_adsap2(const char *, unsigned int,
				      struct sid_data *, struct sid_data *);

int gsp_auth_session_destroy(struct sid_data *);
int gsp_auth_session_destroy_ref_id(unsigned int);

/* Session status and type conversion */
enum sid_status gsp_auth_session_sid_check(const char *);
enum sid_status gsp_auth_session_sdata_check(struct sid_data *);
const char *gsp_auth_session_sid_strerror(enum sid_status);
enum sid_status gsp_auth_session_sid_data(const char *, struct sid_data *);
unsigned int gsp_auth_session_get_ref_id(struct sid_data *);
enum sid_status gsp_auth_session_get_sid_from_ref_id(unsigned int,
						     struct sid_data *);
int gsp_auth_session_connid_inuse(int connid);

/* Session data getters and setters */
int gsp_auth_session_get_num_sessions(void);
int gsp_auth_session_get_num_sessions_free(void);
char *gsp_auth_session_get_username(struct sid_data *);
int gsp_auth_session_set_username(struct sid_data *, const char *);
char *gsp_auth_session_get_sid(struct sid_data *);
char *gsp_auth_session_get_parent_sid(struct sid_data *);
int gsp_auth_session_get_parent_sdata(struct sid_data *, struct sid_data *);
int gsp_auth_session_set_parent_sdata(struct sid_data *, struct sid_data *);
char *gsp_auth_session_get_group_name(struct sid_data *);
int gsp_auth_session_set_group_name(struct sid_data *, const char *);
int gsp_auth_session_add_group_name(struct sid_data *, const char *);
int gsp_auth_session_get_attr_flag(struct sid_data *);
int gsp_auth_session_add_attr_flag(struct sid_data *, int);
int gsp_auth_session_del_attr_flag(struct sid_data *, int);
char *gsp_auth_session_get_auth_type(struct sid_data *);
int gsp_auth_session_set_auth_type(struct sid_data *, const char *);
char *gsp_auth_session_get_curr_auth_type(struct sid_data *);
int gsp_auth_session_set_curr_auth_type(struct sid_data *, const char *);
char *gsp_auth_session_get_ip_addr(struct sid_data *, enum gsp_inet_family *);
int gsp_auth_session_set_ip_addr(struct sid_data *, const char *);
char *gsp_auth_session_get_ipv4_addr(struct sid_data *);
char *gsp_auth_session_get_ipv6_addr(struct sid_data *);
int gsp_auth_session_set_ipv4_addr(struct sid_data *, const char *);
int gsp_auth_session_set_ipv6_addr(struct sid_data *, const char *);
pid_t gsp_auth_session_get_mpid(struct sid_data *);
int gsp_auth_session_set_mpid(struct sid_data *, pid_t);
int gsp_auth_session_get_serial_authorization(struct sid_data *);
int gsp_auth_session_set_serial_authorization(struct sid_data *, int);
int gsp_auth_session_get_outlet_authorization(struct sid_data *);
int gsp_auth_session_set_outlet_authorization(struct sid_data *, int);
int gsp_auth_session_get_tacacs_level(struct sid_data *);
int gsp_auth_session_set_tacacs_level(struct sid_data *, int);
int gsp_auth_session_get_radius_service_type(struct sid_data *);
int gsp_auth_session_set_radius_service_type(struct sid_data *, int);
int gsp_auth_session_get_idle_to(struct sid_data *);
int gsp_auth_session_set_idle_to(struct sid_data *, int);
int gsp_auth_session_get_kill_cnt(struct sid_data *);
int gsp_auth_session_set_kill_cnt(struct sid_data *, int);
int gsp_auth_session_get_et_secs(struct sid_data *);
time_t gsp_auth_session_get_ctime(struct sid_data *);
enum gsp_auth_session_type gsp_auth_session_get_type(struct sid_data *);
int gsp_auth_session_set_type(struct sid_data *, enum gsp_auth_session_type);
enum gsp_auth_target_session_type
gsp_auth_session_get_target_type(struct sid_data *);
int gsp_auth_session_set_target_type(struct sid_data *,
				     enum gsp_auth_target_session_type);
int gsp_auth_session_get_target_id(struct sid_data *);
int gsp_auth_session_set_target_id(struct sid_data *, int);
int gsp_auth_session_get_cid(struct sid_data *, char *);
int gsp_auth_session_set_cid(struct sid_data *, const char *);
int gsp_auth_session_get_adsap2_handle(struct sid_data *);
int gsp_auth_session_get_adsap2_handle_from_privdata(struct gsp_session_privdata *gspd);
int gsp_auth_session_set_adsap2_handle(struct sid_data *, unsigned int);
char *gsp_auth_session_get_adsap2_conn_id(struct sid_data *);
int gsp_auth_session_set_adsap2_conn_id(struct sid_data *, const char *);
uid_t gsp_auth_session_get_uid(struct sid_data *);
int gsp_auth_session_set_uid(struct sid_data *, uid_t);
gid_t gsp_auth_session_get_gid(struct sid_data *);
int gsp_auth_session_set_gid(struct sid_data *, gid_t);
char *gsp_auth_session_get_kill_username(struct sid_data *);
int gsp_auth_session_set_kill_username(struct sid_data *, const char *);
int gsp_auth_session_acl_add(struct sid_data *, const struct gsp_acl_item *);

/* Specialized direct access to shared memory */
int gsp_auth_session_idle_cnt_attach(struct sid_data *, int **);
int gsp_auth_session_idle_cnt_detach(struct sid_data *, void *);

/* Session query and data retrieval */
int gsp_auth_session_get_session_list(libcyc_list **);
int gsp_auth_session_get_sess_username_list(libcyc_list **);
void gsp_auth_session_free_session_list(libcyc_list **);

int gsp_auth_session_get_children_list(struct sid_data *, libcyc_list **);
void gsp_auth_session_free_children_list(libcyc_list **);

int gsp_auth_session_get_data_user(struct sid_data *,
				   struct gsp_session_data *);
void gsp_auth_session_free_data_user(struct gsp_session_data *);

int gsp_auth_session_get_data_priv(struct sid_data *,
				   struct gsp_session_privdata *);
void gsp_auth_session_free_data_priv(struct gsp_session_privdata *);

int gsp_auth_session_get_data_user_list(libcyc_list **);
void gsp_auth_session_free_data_user_list(libcyc_list **);

int gsp_auth_session_get_data_priv_list(libcyc_list **);
void gsp_auth_session_free_data_priv_list(libcyc_list **);

int gsp_auth_session_get_sid_from_target_id(struct sid_data *, int,
					    struct sid_data *);

/*
 * Utility API
 */
int gsp_auth_util_ssh_get_flags(const char *);
int gsp_auth_util_set_authtype_from_pamh(pam_handle_t *, struct sid_data *);
int gsp_auth_util_set_curr_authtype_from_pamh(pam_handle_t *,
					      struct sid_data *);
/* End C GSP Authentication API for applications */

#ifdef __cplusplus
}
#endif

#endif /* GSP_AUTH_H */
