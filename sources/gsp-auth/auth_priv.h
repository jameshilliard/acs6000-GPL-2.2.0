/* -*- c++ -*- */
/*****************************************************************
 * File: auth_priv.h
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: auth_priv.h
 *
 ****************************************************************/

#ifndef AUTH_PRIV_H
#define AUTH_PRIV_H 1

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ctime>

#include <cstdlib>
#include <cstring>

#include <netinet/in.h>
#include <sys/types.h>

#include <security/pam_appl.h>

#include <gsp_auth.h>
#include <gsp_acl.h>
#include <cyc++.h>

namespace gsp_auth {
	enum ftok_proj_ids {
		FTOK_SESSION_TABLE_SHM = 't',
		FTOK_AUTHCNT_SHM = 'r',
		FTOK_SESSION_TABLE_SEMS = 's',
		FTOK_AUTHCNT_SEMS = 'q',
		FTOK_SESSION_SEMS = 'e',
		FTOK_PRIV_SESSION_SEMS = 'p',
		FTOK_USER_SESSION_SEMS = 'u',
	};

	namespace session {
		int priv_gsp_auth_subsession_create(const char *,
						    struct sid_data *,
						    struct sid_data *,
						    unsigned int);
		class pam_helper;
		class sid_info;
		class USER_SHMID;
		class PRIV_SHMID;
		class session_table;
		class shmid_not_found;
		class refid_not_found;
		class sid_not_found;
		class usess_tbl_full;
		class session_data;
		class priv_session_data;
		class user_session_data;
		class session_access;
		class priv_session;
		class user_session;
	}
	
	namespace pam {
		class pam_helper;
		class PAM_USERNAME;
		class PAM_SERVICE_NAME;
		class PAM_TTY_NAME;
		class PAM_CONVERSATION;
		class PAM_RUSER_NAME;
		class PAM_RHOST_NAME;
		class PAM_FLAGS;
		class pam_error;
	}

	namespace util {
		static const int GID_DAEMON = 1; // GID for daemon on appliance
		enum ipc_stat {
			STAT_IPC_ERROR = -1,
			STAT_IPC_CREATE = 0,
			STAT_IPC_CREATE_EXCL,
			STAT_IPC_ATTACH_EXISTING,
		};
		class unique_id;
		class session_id;
		class ssh_authmethod;
		class shmem;
		class sem;
		class auth_counters;
		class delim_parser;

		void shm_destroy(const SHM_ID &);
		key_t get_ipc_key(const PROJECT_ID &);
		gid_t get_default_gid();

		class USERNAME;
		class GROUP_NAME;
		class AUTH_TYPE;
		class CURR_AUTH_TYPE;
		class ADSAP2_HANDLE;
		void update_psd(gsp_auth::session::priv_session_data *,
			        const USERNAME &,
				const AUTH_TYPE &,
				const CURR_AUTH_TYPE &);
		void sess_update_helper(const gsp_auth::session::sid_info &,
					gsp_auth::session::session_table &,
					const SHM_ID &,
					gsp_auth::session::session_access &);
	}

	namespace group {
		class remote_group;
	}

	namespace public_api {
		class api_helper;
	}

	class libinfo;
}

// Info about this library
class gsp_auth::libinfo {
	static const std::string LIB_NAME;
	static const std::string LIB_VERSION;
	static const std::string LIB_LICENSE;
public:
	static std::string get_name() { return(LIB_NAME); }
	static std::string get_version() { return(LIB_VERSION); }
	static std::string get_license() { return(LIB_LICENSE); }
};

class gsp_auth::util::shmem : public cyc_shmem {
public:
	shmem(const PROJECT_ID &proj_id) :
		cyc_shmem(IPC_KEY(gsp_auth::util::get_ipc_key(proj_id))) {}
	enum ipc_stat screate(const SHM_SZ &, const SHM_FLAG &);
};

class gsp_auth::util::sem : public cyc_sem {
	static const int INIT_VAL;
public:
	sem(const PROJECT_ID &proj_id) :
		cyc_sem(IPC_KEY(gsp_auth::util::get_ipc_key(proj_id))) {}
	enum ipc_stat screate(const SEM_NUM_SEMS &, const SEM_FLAG &);
	int op(const SEM_NUM &num, const SEM_OP &op) {
		return(cyc_sem::op(num, op, SEM_FLAG::UNDO()));
	}
};

class gsp_auth::session::sid_info {
	static const std::string NULL_SID_DATA_ERR;

	sid_data *sdata;
	bool sdata_allocated;
public:
	sid_info &operator=(const sid_info &);
	bool operator<(const sid_info &rhs) const {
		return(get_sid_str() < rhs.get_sid_str());
	};
	bool operator==(const sid_info &) const;
	bool operator!=(const sid_info &other) const {
		return(! (*this == other));
	}

	sid_info(const sid_info &);
	sid_info(sid_data *s) : sdata(s), sdata_allocated(false) {
		if (s == 0) throw app_error(NULL_SID_DATA_ERR);
	}
	sid_info() : sdata(0), sdata_allocated(true) {
		sdata = new sid_data;

		memset(sdata->sid, '\0', sizeof(sdata->sid));
		sdata->sid_sz = 0;
	}
	~sid_info() {
		if (sdata && sdata_allocated) {
			delete sdata;
			sdata = 0;
		}
	}
	sid_data *get_sdata() { return(sdata); }
	const char *get_sid() const { return(sdata->sid); }
	std::string get_sid_str() const;
	static int get_max_sid_sz() { return(SID_SZ); }
	int get_sid_sz() const { return(sdata->sid_sz); }
	void set_sid(std::string);
	void set_sid_sz(int sz) { sdata->sid_sz = sz; }
};

std::ostream &operator<<(std::ostream &, const gsp_auth::session::sid_info &);
std::istream &operator>>(std::istream &, gsp_auth::session::sid_info &);

class gsp_auth::util::unique_id {
	static int random_number_generate();
	static void random_sequence_generate(char *, int);
public:
	static std::string generate(const char *, int);
	static std::string generate();
};

class gsp_auth::util::session_id : private gsp_auth::util::unique_id {
	static const std::string NULL_SID_DATA_ERR;

	gsp_auth::session::sid_info *sinfo;

	session_id(const session_id &);
	session_id &operator=(const session_id &);
public:
	session_id(gsp_auth::session::sid_info *s) : sinfo(s) {}
	void generate(std::string);
};

struct gsp_auth::util::USERNAME {
	explicit USERNAME(std::string s) : val(s) {}
	explicit USERNAME(const char *s) : val(s ? s : "") {}
	std::string val;
};

struct gsp_auth::util::GROUP_NAME {
	explicit GROUP_NAME(std::string s) : val(s) {}
	explicit GROUP_NAME(const char *s) : val(s ? s : "") {}
	std::string val;
};

struct gsp_auth::util::ADSAP2_HANDLE {
	explicit ADSAP2_HANDLE(unsigned int s) : val(s) {}
	unsigned int val;
};

struct gsp_auth::util::AUTH_TYPE {
	explicit AUTH_TYPE(std::string s) : val(s) {}
	explicit AUTH_TYPE(const char *s) : val(s ? s : "") {}
	std::string val;
};

struct gsp_auth::util::CURR_AUTH_TYPE {
	explicit CURR_AUTH_TYPE(std::string s) : val(s) {}
	explicit CURR_AUTH_TYPE(const char *s) : val(s ? s : "") {}
	std::string val;
};

class gsp_auth::pam::pam_error : public app_error {
	int ret;
public:
	pam_error(int r, std::string s) : app_error(s), ret(r) {}
	int get_ret() { return(ret); }
};

struct gsp_auth::pam::PAM_SERVICE_NAME {
	explicit PAM_SERVICE_NAME(std::string s) : val(s) {}
	std::string val;
};

struct gsp_auth::pam::PAM_USERNAME {
	explicit PAM_USERNAME(std::string s) : val(s) {}
	std::string val;
};

struct gsp_auth::pam::PAM_TTY_NAME {
	explicit PAM_TTY_NAME(std::string s) : val(s) {}
	std::string val;
};

struct gsp_auth::pam::PAM_CONVERSATION {
	explicit PAM_CONVERSATION(const pam_conv *c) : val(c) {}
	const pam_conv *val;
};

struct gsp_auth::pam::PAM_RUSER_NAME {
	explicit PAM_RUSER_NAME(std::string s) : val(s) {}
	std::string val;
};

struct gsp_auth::pam::PAM_RHOST_NAME {
	explicit PAM_RHOST_NAME(std::string s) : val(s) {}
	std::string val;
};

struct gsp_auth::pam::PAM_FLAGS {
	explicit PAM_FLAGS(int f) : val(f) {}
	int val;
};

class gsp_auth::pam::pam_helper {
	static const std::string SERVICE_NAME_DEFAULT;
	static const std::string NULL_PAMH_ERR;
	static const std::string NULL_SID_ERR;
	static const std::string PAMH_SID_UNAVAIL_ERR;

	pam_handle_t *pamh;
	bool d_pam_end;

	void check_errcode(int ret) const {
		if (ret != PAM_SUCCESS) {
			std::ostringstream err_msg;
			err_msg << __func__ << ": ";
			err_msg << pam_strerror(pamh, ret);

			throw pam_error(ret, err_msg.str());
		}
	}

	pam_helper(const pam_helper &);
	pam_helper &operator=(const pam_helper &);
public:
	/* Initialize the class with an existing PAM session */
	pam_helper(pam_handle_t *p) : pamh(p), d_pam_end(false) {
		if (p == 0) throw app_error(NULL_PAMH_ERR);
	}
	/* Create a new PAM session */
	pam_helper(const PAM_SERVICE_NAME &,
		   const PAM_USERNAME &,
		   const PAM_TTY_NAME &,
		   const PAM_CONVERSATION &);
	/* Destroy the PAM session in destructor, if desired */
	~pam_helper() {
		if (d_pam_end) {
			end();
		}
	}

	pam_handle_t *get_pamh() { return(pamh); }

	/* End the PAM session */
	void end() { end(PAM_SUCCESS); }
	void end(int);

	/* Modify this class's behavior */
	void set_pam_end_on_destroy(bool b) { d_pam_end = b; }

	/* Specific get/set item */
	std::string get_username() const;
	std::string get_orig_service() const;
	std::string get_service() const;
	void get_sid(sid_data *) const;
	void set_ruser(const PAM_RUSER_NAME &);
	void set_rhost(const PAM_RHOST_NAME &);
	void set_service(const PAM_SERVICE_NAME &);

	/* Generic get/set item */
	void get_item(int, const void **) const;
	void set_item(int, const void *);

	/* Authentication methods */
	void authenticate(const PAM_FLAGS &);
	void authenticate() { authenticate(PAM_FLAGS(0)); }
	void acct_mgmt(const PAM_FLAGS &);
	void acct_mgmt() { acct_mgmt(PAM_FLAGS(0)); }
	void chauthtok(const PAM_FLAGS &);
	void setcred(const PAM_FLAGS &);
	void open_session(const PAM_FLAGS &);
	void open_session() { open_session(PAM_FLAGS(0)); }
	void close_session(const PAM_FLAGS &);
	void close_session() { close_session(PAM_FLAGS(0)); }
};

class gsp_auth::util::delim_parser {
	static const std::string EMPTY_STR_ERR;

	char DELIMITER;
	char TERMINATOR;
	std::string raw_str;
	std::list<std::string> parsed_items;
public:
	delim_parser(std::string rstr, char delim, char term = 0) :
		DELIMITER(delim), TERMINATOR(term), raw_str(rstr) {}
	~delim_parser() {}

	std::string get_raw_str() const { return(raw_str); }
	std::list<std::string> &get_parsed_items() { return(parsed_items); }
	void parse();
	void unparse();
};

class gsp_auth::group::remote_group : private gsp_auth::util::delim_parser {
	static const char GROUP_NAME_DELIMITER;
	static const char GROUP_NAME_TERMINATOR;
public:
	remote_group(std::string gn) : delim_parser(gn, GROUP_NAME_DELIMITER,
						    GROUP_NAME_TERMINATOR) {}

	std::string get_group_name() const { return(get_raw_str()); }
	std::list<std::string> &get_groups() { return(get_parsed_items()); }
	void parse() { gsp_auth::util::delim_parser::parse(); }
	void unparse() { gsp_auth::util::delim_parser::unparse(); }
};

class gsp_auth::util::ssh_authmethod {
	struct am_attr {
		int flags;
	};

	std::map<std::string, am_attr> ssham;
public:
	ssh_authmethod();

	int get_flags(const std::string &);
};

class gsp_auth::session::shmid_not_found : public app_error {
public:
	shmid_not_found(std::string s) : app_error(s) {}
};

class gsp_auth::session::refid_not_found : public app_error {
public:
	refid_not_found(std::string s) : app_error(s) {}
};

class gsp_auth::session::sid_not_found : public app_error {
public:
	sid_not_found(std::string s) : app_error(s) {}
};

struct gsp_auth::session::USER_SHMID : public SHM_ID {
public:
	explicit USER_SHMID(int n) : SHM_ID(n) {}
};
struct gsp_auth::session::PRIV_SHMID : public SHM_ID {
	explicit PRIV_SHMID(int n) : SHM_ID(n) {}
};

class gsp_auth::session::usess_tbl_full : public app_error {
	int user_shmid;
	int priv_shmid;
public:
	usess_tbl_full(const USER_SHMID &us, const PRIV_SHMID &ps,
		       std::string str) :
		app_error(str), user_shmid(us.val), priv_shmid(ps.val) {}
	int get_user_shmid() { return(user_shmid); }
	int get_priv_shmid() { return(priv_shmid); }
};

class gsp_auth::session::session_table {
	static const int MAX_SESSIONS;
	static const std::string SHMID_LOOKUP_FAILED_ERR;
	static const std::string REFID_LOOKUP_FAILED_ERR;
	static const std::string SID_LOOKUP_FAILED_ERR;
	static const std::string USESS_TBL_FULL_ERR;
	static const std::string NOT_AUTHORIZED_ACCESS_ERR;
	static const std::string NOT_AUTHORIZED_CREATE_ERR;
	static const std::string INVALID_SHMID_ERR;
	static const std::string INVALID_REFID_ERR;

	enum sem_ids {
		SEM_SESSION_TBL_CREATE = 0,
		SEM_SESSION_TBL_LOCK,
	};

	struct utable_data {
		unsigned int ref;
		char sid[SID_SZ];
		int user_shmid;
		int priv_shmid;
	};

	gsp_auth::util::sem *sem;
	gsp_auth::util::shmem *ut;
	utable_data *utable;

	void lock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_SESSION_TBL_LOCK), SEM_OP::PEND());
		}
	}
	void unlock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_SESSION_TBL_LOCK), SEM_OP::REL());
		}
	}

	session_table(const session_table &);
	session_table &operator=(const session_table &);
public:
	session_table();
	~session_table() {
		if (ut) {
			try {
				ut->detach();
			} catch (...) {}
			delete ut;
			ut = 0;
			utable = 0;
		}
		if (sem) {
			delete sem;
			sem = 0;
		}
	}

	/* Add an entry */
	void add(const sid_info &, const USER_SHMID &, const PRIV_SHMID &);
	void update(const sid_info &, const USER_SHMID &, const PRIV_SHMID &);
	void remove(const sid_info &); // Remove an entry
	int lookup_user_shmid(const sid_info &); // Get user shmid for a sid
	int lookup_priv_shmid(const sid_info &); // Get priv shmid for a sid
	unsigned int lookup_ref_id(const sid_info &); // Get ref id for a sid
	std::string lookup_sid(unsigned int); // Get an sid string for a ref id
	int get_num_sessions(); // Number of sessions in use
	int get_num_sessions_free(); // Number of free sessions

	/* Return a map containing SID/shmid associations for the user
	 * and private tables.  Other classes may use this information
	 * to talk through the shared memory. */
	std::map<std::string, int> get_user_stable();
	std::map<std::string, int> get_priv_stable();
	/* Map with SID/refid associations */
	std::map<std::string, unsigned int> get_refid_stable();
	/* Sometimes, we just need a list of SIDs */
	std::vector<std::string> get_sids();
};

class gsp_auth::util::auth_counters {
	static const int INITIAL_REF_ID;
	static const int INITIAL_UID;
	static const std::string NOT_AUTHORIZED_CREATE_ERR;

	enum sem_ids {
		SEM_AUTHCNT_CREATE = 0,
		SEM_AUTHCNT_LOCK,
	};

	struct counter_data {
		unsigned int ref_id;
		uid_t next_uid;
	};

	gsp_auth::util::sem *s;
	gsp_auth::util::shmem *r;
	counter_data *cdata;

	void lock() {
		if (s) {
			s->op(SEM_NUM(SEM_AUTHCNT_LOCK), SEM_OP::PEND());
		}
	}
	void unlock() {
		if (s) {
			s->op(SEM_NUM(SEM_AUTHCNT_LOCK), SEM_OP::REL());
		}
	}

	auth_counters(const auth_counters &);
	auth_counters &operator=(const auth_counters &);
public:
	auth_counters();
	~auth_counters() {
		if (r) {
			try {
				r->detach();
			} catch (...) {}
			cdata = 0;
			delete r;
			r = 0;
		}
		if (s) {
			delete s;
			s = 0;
		}
	}
	unsigned int get_ref_id();
	unsigned int get_ref_id_raw() { return(cdata->ref_id); }
	uid_t get_next_uid();
	uid_t get_next_uid_raw() { return(cdata->next_uid); }
};

/* This is a base class for the data object that stores the data for a
 * user's session.  There are getter and setter methods for the data as
 * well as handling for copies and assignment.  There are two use cases:
 * in one use case, this class will allocated the memory for the data
 * structure used to hold the values (and free it when the object is
 * destroyed); in the other case, the user cann pass in a pointer to the
 * data structure used to hold the values (in this case, the class will
 * not allocate or free the data structure). */
class gsp_auth::session::session_data {
public:
	virtual ~session_data() {}

	virtual sid_data *get_sid() = 0;
	virtual void set_sid(const sid_data *) = 0;
};


class gsp_auth::session::priv_session_data : public session_data {
	static const int IDLE_TIMEOUT_DEFAULT; // default idle timeout in secs
	static const int KILL_CNT_DEFAULT; // default for kill counter
	static const std::string INVALID_CID_SZ_ERR;
	static const std::string INVALID_AUTH_TYPE_SZ_ERR;

	gsp_session_privdata *gspd;
	bool gspd_allocated;

	/* ACLs should be allocated with new; they can be added from the
	 * restore code or placed there (e.g. using the API or via
	 * authorization code in the core) */
	void clear_acls(libcyc_list **acls) {
		gsp_acl_item *acl = 0;

		libcyc_ll_reset(*acls);
		
		while ((acl = static_cast<gsp_acl_item *>
			(libcyc_ll_iterate(*acls)))) {
			delete acl;
		}

		libcyc_ll_free(*acls);
		*acls = 0;
	}
public:
	void update_sess_ctime() { gspd->attr.sess_ctime = time(0); }
	void clear_auth_acls() {
		if (gspd && gspd->session_acls.auth_acls) {
			clear_acls(&gspd->session_acls.auth_acls);
		}
	}
	void clear_cfg_acls() {
		if (gspd && gspd->session_acls.cfg_acls) {
			clear_acls(&gspd->session_acls.cfg_acls);
		}
	}

	priv_session_data &operator=(const priv_session_data &);

	priv_session_data(const priv_session_data &);
	priv_session_data() : gspd(0), gspd_allocated(true) {
		gspd = new gsp_session_privdata;

		gspd->uinfo.username = 0;
		gspd->uinfo.username_sz = 0;
		gspd->ginfo.group_name = 0;
		gspd->ginfo.group_name_sz = 0;

		memset(gspd->sid.sid, '\0', sizeof(gspd->sid.sid));
		gspd->sid.sid_sz = 0;
		memset(gspd->parent_sid.sid, '\0',
		       sizeof(gspd->parent_sid.sid));
		gspd->parent_sid.sid_sz = 0;

		gspd->attr.flags = 0;
		memset(gspd->attr.auth_type, '\0',
		       sizeof(gspd->attr.auth_type));
		memset(gspd->attr.curr_auth_type, '\0',
		       sizeof(gspd->attr.curr_auth_type));
		gspd->attr.mpid = -1;
		gspd->attr.idle_to = IDLE_TIMEOUT_DEFAULT;
		gspd->attr.kill_cnt = KILL_CNT_DEFAULT;
		gspd->attr.sess_type = GSP_AUTH_SESS_UNKNOWN;
		gspd->attr.targ_sess_type = GSP_AUTH_TARG_UNKNOWN;
		gspd->attr.target_id = -1;
		memset(gspd->attr.cid, '\0', sizeof(gspd->attr.cid));
		gspd->attr.uid = ~0;
		gspd->attr.gid = ~0;
		memset(&gspd->attr.ipaddr, 0,
		       sizeof(gspd->attr.ipaddr));
		gspd->attr.ipaddr_fam = GSP_AF_UNKNOWN;
		gspd->adsap2_tok.handle = -1;
		memset(gspd->adsap2_tok.conn_id, '\0',
		       sizeof(gspd->adsap2_tok.conn_id));
		gspd->session_acls.auth_acls = 0;
		gspd->session_acls.cfg_acls = 0;
		memset(gspd->attr.kill_username, '\0',
		       sizeof(gspd->attr.kill_username));

		update_sess_ctime();
	}
	priv_session_data(gsp_session_privdata *s) : gspd(s),
						     gspd_allocated(false) {}
	~priv_session_data() {
		if ((gspd == 0) || (gspd_allocated == false)) return;

		if (gspd->uinfo.username) {
			free((void *)gspd->uinfo.username);
			gspd->uinfo.username = 0;
			gspd->uinfo.username_sz = 0;
		}
		if (gspd->ginfo.group_name) {
			free((void *)gspd->ginfo.group_name);
			gspd->ginfo.group_name = 0;
			gspd->ginfo.group_name_sz = 0;
		}

		clear_auth_acls();
		clear_cfg_acls();

		delete gspd;
		gspd = 0;
	}

	/* Access to private data members */
	gsp_session_privdata *get_gspd() { return(gspd); }
	bool get_gspd_allocated() { return(gspd_allocated); }
	gsp_auth_privattr *get_attr() { return(&gspd->attr); }
	gsp_auth_adsap2_token *get_adsap2_tok() { return(&gspd->adsap2_tok); }
	sid_data *get_sid() { return(&gspd->sid); }
	gsp_acl_session *get_acl_session() { return(&gspd->session_acls); }

	/* SID handling */
	void set_sid(const sid_data *s) { if (s) gspd->sid = *s; }
	sid_data *get_parent_sid() { return(&gspd->parent_sid); }
	void set_parent_sid(const sid_data *s) {
		if (s) gspd->parent_sid = *s;
	}
	bool has_parent() {
		if ((gspd->parent_sid.sid[0] == '\0') &&
		    (gspd->parent_sid.sid_sz == 0)) {
			return(false);
		}

		return(true);
	}

	/* User name */
	std::string get_username() const {
		if (gspd->uinfo.username) return(gspd->uinfo.username);

		return("");
	}
	void set_username(const char *);
	void set_username(std::string s) { set_username(s.c_str()); }

	/* Group name */
	std::string get_group_name() const {
		if (gspd->ginfo.group_name) return(gspd->ginfo.group_name);

		return("");
	}
	void add_group_name(const char *);
	void set_group_name(const char *);
	void set_group_name(std::string s) { set_group_name(s.c_str()); }

	/* Attributes */
	int get_flags() const { return(gspd->attr.flags); }
	void set_flag(int a) { gspd->attr.flags = a; }
	void add_flag(int a) { gspd->attr.flags |= a; }
	void del_flag(int a) { gspd->attr.flags &= ~a; }
	std::string get_auth_type() const {
		if (gspd->attr.auth_type[0] == '\0') return("");

		return(gspd->attr.auth_type);
	}
	void set_auth_type(std::string);
	void set_auth_type(const char *s) {
		set_auth_type(s ? std::string(s) : "");
	}
	std::string get_curr_auth_type() const {
		if (gspd->attr.curr_auth_type[0] == '\0') return("");
		
		return(gspd->attr.curr_auth_type);
	}
	void set_curr_auth_type(std::string);
	void set_curr_auth_type(const char *s) {
		set_curr_auth_type(s ? std::string(s) : "");
	}
	pid_t get_mpid() const { return(gspd->attr.mpid); }
	void set_mpid(pid_t pid) { gspd->attr.mpid = pid; }
	int get_radius_service_type() const { return(gspd->attr.radius_service_type); }
	void set_radius_service_type(int sType) { gspd->attr.radius_service_type = sType; }
	int get_serial_authorization() const { return(gspd->attr.serial_authorization); }
	void set_serial_authorization(int st) { gspd->attr.serial_authorization = st; }
	int get_outlet_authorization() const { return(gspd->attr.outlet_authorization); }
	void set_outlet_authorization(int st) { gspd->attr.outlet_authorization = st; }
	int get_tacacs_level() const { return(gspd->attr.tacacs_level); }
	void set_tacacs_level(int level) { gspd->attr.tacacs_level = level; }
	int get_idle_to() const { return(gspd->attr.idle_to); }
	void set_idle_to(int to) { gspd->attr.idle_to = to; }
	int get_kill_cnt() const { return(gspd->attr.kill_cnt); }
	void set_kill_cnt(int cnt) { gspd->attr.kill_cnt = cnt; }
	time_t get_sess_ctime() const { return(gspd->attr.sess_ctime); }
	enum gsp_auth_session_type get_session_type() const {
		return(gspd->attr.sess_type);
	}
	void set_session_type(enum gsp_auth_session_type stype) {
		gspd->attr.sess_type = stype;
	}
	enum gsp_auth_target_session_type get_target_session_type() const {
		return(gspd->attr.targ_sess_type);
	}
	void
	set_target_session_type(enum gsp_auth_target_session_type tstype) {
		gspd->attr.targ_sess_type = tstype;
	}
	int get_target_id() const { return(gspd->attr.target_id); }
	void set_target_id(int tid) { gspd->attr.target_id = tid; }
	std::string get_cid() const;
	void set_cid(std::string);
	uid_t get_uid() const { return(gspd->attr.uid); }
	void set_uid(int uid) { gspd->attr.uid = uid; }
	gid_t get_gid() const { return(gspd->attr.gid); }
	void set_gid(int gid) { gspd->attr.gid = gid; }
	std::string get_kill_username() const {
		return(gspd->attr.kill_username);
	}
	void set_kill_username(std::string);
	void set_kill_username(const char *s) {
		set_kill_username(s ? std::string(s) : "");
	}

	/* IPv4 and IPv6 IP address get/set */
	enum gsp_inet_family get_ipaddr_fam() const {
		return(gspd->attr.ipaddr_fam);
	}
	std::string get_ipv4_addr() const;
	void set_ipv4_addr(std::string);
	std::string get_ipv6_addr() const;
	void set_ipv6_addr(std::string);

	/* ADSAP2 Token */
	unsigned int get_adsap2_handle() const {
		return(gspd->adsap2_tok.handle);
	}
	void set_adsap2_handle(int h) {
		gspd->adsap2_tok.handle = h;
	}
	std::string get_adsap2_conn_id() const {
		return(gspd->adsap2_tok.conn_id);
	}
	void set_adsap2_conn_id(std::string);
	void set_adsap2_conn_id(const char *s) {
		set_adsap2_conn_id(s ? std::string(s) : "");
	}
};

class gsp_auth::session::user_session_data : public session_data {
	gsp_session_data *gsd;
	bool gsd_allocated;
public:
	user_session_data &operator=(const user_session_data &);
	user_session_data(const user_session_data &);
	user_session_data() : gsd(0), gsd_allocated(true) {
		gsd = new gsp_session_data;

		memset(gsd, 0, sizeof(gsp_session_data));
		memset(gsd->sid.sid, '\0', sizeof(gsd->sid.sid));
		gsd->sid.sid_sz = 0;

		gsd->attr.idle_cnt = 0;
	}
	user_session_data(gsp_session_data *s) : gsd(s),
						 gsd_allocated(false) {}
	~user_session_data() {
		if (gsd && gsd_allocated) {
			delete gsd;
			gsd = 0;
		}
	}

	/* Access to private data members */
	gsp_session_data *get_gsd() { return(gsd); }
	bool get_gsd_allocated() { return(gsd_allocated); }
	gsp_auth_attr *get_attr() { return(&gsd->attr); }

	/* SID: Use the sid_info class to manage this structure */
	sid_data *get_sid() { return(&gsd->sid); }
	void set_sid(const sid_data *s) { if (s) gsd->sid = *s; }

	/* Attributes */
	int get_idle_cnt() const { return(gsd->attr.idle_cnt); }
	void set_idle_cnt(int to) { gsd->attr.idle_cnt = to; }
	void reset_idle_cnt() { gsd->attr.idle_cnt = 0; }
};

class gsp_auth::session::session_access {
public:
	virtual ~session_access() {}

	virtual int shm_save() = 0;
	virtual void shm_restore(const SHM_ID &) = 0;
	virtual int shm_update(const SHM_ID &) = 0;
	virtual void shm_update_perms(const SHM_ID &) = 0;
};

class gsp_auth::session::priv_session : public session_access {
	static const std::string NULL_SESSION_ERR;

	static const char auth_aclsep;
	static const char cfg_aclsep;

	priv_session_data *session;
	bool session_allocated;
	gsp_auth::util::sem *sem;

	enum sem_ids {
		SEM_PRIV_SESSION_LOCK,
	};

	void lock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_PRIV_SESSION_LOCK),
				SEM_OP::PEND());
		}
	}
	void unlock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_PRIV_SESSION_LOCK),
				SEM_OP::REL());
		}
	}
	
	size_t get_session_sz(); // Help out shm_save()
	void marshal(char *);
	void unmarshal(const char *, size_t);
	void init();

	priv_session(const user_session &);
	priv_session &operator=(const user_session &);
public:
	priv_session(priv_session_data *s) : session(s),
					     session_allocated(false),
					     sem(0) {
		init();
	}
	priv_session() : session(0), session_allocated(false), sem(0) {
		init();
	}
	~priv_session() {
		if (session && session_allocated) {
			delete session;
			session = 0;
		}
		if (sem) {
			delete sem;
			sem = 0;
		}
	}

	void set(priv_session_data *s) {
		if (session && session_allocated) {
			delete session;
		}
		session = s;
	}
	priv_session_data *get_session() { return(session); }

	int shm_save();
	void shm_restore(const SHM_ID &);
	int shm_update(const SHM_ID &);
	void shm_update_perms(const SHM_ID &shmid) {}
};

class gsp_auth::session::user_session : public session_access {
	static const std::string NULL_SESSION_ERR;

	user_session_data *session;
	bool session_allocated;
	gsp_auth::util::sem *sem;
	std::string username;

	enum sem_ids {
		SEM_USER_SESSION_LOCK,
	};

	void lock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_USER_SESSION_LOCK),
				SEM_OP::PEND());
		}
	}
	void unlock() {
		if (sem) {
			sem->op(SEM_NUM(SEM_USER_SESSION_LOCK), SEM_OP::REL());
		}
	}
	void init();

	user_session(const user_session &);
	user_session &operator=(const user_session &);
public:
	user_session(const gsp_auth::util::USERNAME &un,
		     user_session_data *s) : session(s),
					     session_allocated(false),
					     sem(0), username(un.val) {
		init();
	}
	user_session(std::string u) : session(0), session_allocated(false),
				      sem(0), username(u) {
		init();
	}
	user_session() : session(0), session_allocated(false),
			 sem(0), username("") {
		init();
	}
	~user_session() {
		if (session && session_allocated) {
			delete session;
			session = 0;
		}
		if (sem) {
			delete sem;
			sem = 0;
		}
	}

	void set(user_session_data *s) {
		if (session && session_allocated) {
			delete session;
		}
		session = s;
	}
	user_session_data *get_session() { return(session); }

	int shm_save();
	void shm_restore(const SHM_ID &);
	int shm_update(const SHM_ID &);
	void shm_update_perms(const SHM_ID &);
};

class gsp_auth::public_api::api_helper {
	gsp_auth::util::sem *s;

	static const std::string NOT_AUTH_APIH_SEM_ERR;

	enum sess_sem_ids {
		SEM_SESSION_CREATE = 0,
		SEM_SESSION_LOCK,
	};

	api_helper(const api_helper &);
	api_helper &operator=(const api_helper &);
public:
	api_helper();
	~api_helper() {
		if (s) {
			delete s;
			s = 0;
		}
	}

	void lock_create() {
		if (s) {
			s->op(SEM_NUM(SEM_SESSION_CREATE), SEM_OP::PEND());
		}
	}
	void unlock_create() {
		if (s) {
			s->op(SEM_NUM(SEM_SESSION_CREATE), SEM_OP::REL());
		}
	}
	void lock() {
		if (s) {
			s->op(SEM_NUM(SEM_SESSION_LOCK), SEM_OP::PEND());
		}
	}
	void unlock() {
		if (s) {
			s->op(SEM_NUM(SEM_SESSION_LOCK), SEM_OP::REL());
		}
	}
};
#endif /* AUTH_PRIV_H */
