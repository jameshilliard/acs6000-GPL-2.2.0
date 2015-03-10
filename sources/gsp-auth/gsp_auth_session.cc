/*****************************************************************
 * File: gsp_auth_session.c
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_auth_session.c
 *
 ****************************************************************/

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <security/pam_appl.h>

#include <gsp_auth.h>

#include <cyc++.h>
#include "auth_priv.h"

/* 
 * Global operators
 */
/* Write the data in the sid_info structure to a stream */
std::ostream &operator<<(std::ostream &os,
			 const gsp_auth::session::sid_info &sinfo)
{
	const char *sid = sinfo.get_sid();
	int sid_sz = sinfo.get_sid_sz();

        if (sid_sz > 0) {
                os.write(sid, sid_sz);
        }

        return(os);
}

/* Populate the sid_info structure with data from a stream */
std::istream &operator>>(std::istream &is,
			 gsp_auth::session::sid_info &sinfo)
{
        std::streamsize size;
        std::streambuf *ibuf;
	const int max_sid_sz = sinfo.get_max_sid_sz();
	sid_data *sdata = sinfo.get_sdata();

        ibuf = is.rdbuf();
        size = ibuf->in_avail();

	if (size > max_sid_sz) size = max_sid_sz;

        ibuf->sgetn(sdata->sid, size);
        sinfo.set_sid_sz(size);

        return(is);
}

const std::string gsp_auth::session::sid_info::
NULL_SID_DATA_ERR = "sid_data not set";

gsp_auth::session::sid_info &
gsp_auth::session::sid_info::operator=(const sid_info &rhs)
{
	if (this != &rhs) { // avoid self-assignment
		if (sdata && sdata_allocated) {
			/* Remove old resources */
			delete sdata;
			sdata = 0;
		}

		sdata = new sid_data;
		sdata_allocated = true;

		/* Same as copy constructor */
		*sdata = *(rhs.sdata);
	}

	return(*this);
}

bool gsp_auth::session::sid_info::operator==(const sid_info &other) const
{
	if ((memcmp(sdata->sid, other.sdata->sid, SID_SZ) == 0) &&
	    (sdata->sid_sz == other.sdata->sid_sz)) {
		return(true);
	}

	return(false);
}

gsp_auth::session::sid_info::sid_info(const sid_info &rhs) :
	sdata(0), sdata_allocated(true)
{
	sdata = new sid_data;
	
	/* Same as copy assignment operator */
	*sdata = *(rhs.sdata);
}

std::string gsp_auth::session::sid_info::get_sid_str() const
{
	if (sdata == 0) throw app_error(NULL_SID_DATA_ERR);

	std::ostringstream s;

	s.write(sdata->sid, sdata->sid_sz);

	return(s.str());
}

void gsp_auth::session::sid_info::set_sid(std::string s)
{
	if (sdata == 0) throw app_error(NULL_SID_DATA_ERR);

	int size = s.size();
	if (size > get_max_sid_sz()) size = get_max_sid_sz();

	memcpy(sdata->sid, s.c_str(), size);
	sdata->sid_sz = size;
}

const int gsp_auth::session::session_table::MAX_SESSIONS = 512;
const std::string gsp_auth::session::session_table::
SHMID_LOOKUP_FAILED_ERR = "Unable to locate session in shm table";
const std::string gsp_auth::session::session_table::
REFID_LOOKUP_FAILED_ERR = "Unable to locate reference ID in shm table";
const std::string gsp_auth::session::session_table::
SID_LOOKUP_FAILED_ERR = "Unable to locate SID in shm table";
const std::string gsp_auth::session::session_table::
USESS_TBL_FULL_ERR = "Session ID table full";
const std::string gsp_auth::session::session_table::
NOT_AUTHORIZED_ACCESS_ERR = "Not authorized to access private session data";
const std::string gsp_auth::session::session_table::
NOT_AUTHORIZED_CREATE_ERR = "Not authorized to create the session table";
const std::string gsp_auth::session::session_table::
INVALID_SHMID_ERR = "At least one valid shmid must be specified";
const std::string gsp_auth::session::session_table::
INVALID_REFID_ERR = "Invalid reference ID";

gsp_auth::session::session_table::session_table() : sem(0), ut(0), utable(0)
{
	bool clear_shmem = false;
	uid_t uid = geteuid();
	enum gsp_auth::util::ipc_stat ret;
	
	/* Don't even bother to create the semaphore if we're not
	   root */
	if (uid == 0) {
		// Set up the semaphores we'll need
		sem = new gsp_auth::util::
			sem(PROJECT_ID(FTOK_SESSION_TABLE_SEMS));
		ret = sem->screate(SEM_NUM_SEMS(2), SEM_FLAG(0644));
		if (ret == gsp_auth::util::STAT_IPC_CREATE_EXCL) {
			clear_shmem = true;
		}

		// Make sure we're initializing one-at-a-time
		sem->op(SEM_NUM(SEM_SESSION_TBL_CREATE), SEM_OP::PEND());
	}

	try {
		/* Attach to user table in shared memory; create it if
		 * it doesn't exist */
		ut = new gsp_auth::util::
			shmem(PROJECT_ID(FTOK_SESSION_TABLE_SHM));
		
		ret = ut->screate(SHM_SZ(MAX_SESSIONS * sizeof(utable_data)),
				  SHM_FLAG((uid == 0) ? 0644 : 0444));
		if (ret == gsp_auth::util::STAT_IPC_CREATE_EXCL) {
			if (uid != 0) {
				/* Non-root user is trying to create the table;
				   not good */
				ut->destroy();
				throw app_error(NOT_AUTHORIZED_CREATE_ERR);
			}
		}
		void *mem = ut->attach();
		utable = new (mem) utable_data[MAX_SESSIONS];
		
		if (clear_shmem) {
			for (int i = 0; i < MAX_SESSIONS; i++) {
				utable[i].ref = 0;
				utable[i].sid[0] = '\0';
				utable[i].user_shmid = ERROR;
				utable[i].priv_shmid = ERROR;
			}
		}
	} catch (...) {
		if (uid == 0) {
			sem->op(SEM_NUM(SEM_SESSION_TBL_CREATE),
				SEM_OP::REL());
		}
		throw;
	}
	
	/* We're done initializing */
	if (uid == 0) {
		sem->op(SEM_NUM(SEM_SESSION_TBL_CREATE), SEM_OP::REL());
	}
}

void gsp_auth::session::session_table::add(const sid_info &sinfo,
					   const USER_SHMID &ushmid,
					   const PRIV_SHMID &pshmid)
{
	bool found = false;

	if ((ushmid.val == ERROR) && (pshmid.val == ERROR)) {
		throw app_error(INVALID_SHMID_ERR);
	}

	lock();

	gsp_auth::util::auth_counters r;

	for (int i = 0; i < MAX_SESSIONS; i++) {
		/* New entry goes in the first available row */
		if ((utable[i].ref == 0) &&
		    (utable[i].sid[0] == '\0') &&
		    (utable[i].user_shmid == ERROR) &&
		    (utable[i].priv_shmid == ERROR)) {
			utable[i].ref = r.get_ref_id();
			memcpy(utable[i].sid, sinfo.get_sid(), SID_SZ);
			utable[i].user_shmid = ushmid.val;
			utable[i].priv_shmid = pshmid.val;
			found = true;
			break;
		}
	}

	unlock();

	if (! found) {
		throw usess_tbl_full(ushmid, pshmid,
				     USESS_TBL_FULL_ERR);
	}
}

void gsp_auth::session::session_table::update(const sid_info &sinfo,
					      const USER_SHMID &ushmid,
					      const PRIV_SHMID &pshmid)
{
	bool found = false;

	if ((ushmid.val == ERROR) && (pshmid.val == ERROR)) {
		throw app_error(INVALID_SHMID_ERR);
	}

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (memcmp(utable[i].sid, sinfo.get_sid(), SID_SZ) == 0) {
			utable[i].user_shmid = ushmid.val;
			utable[i].priv_shmid = pshmid.val;
			found = true;
			break;
		}
	}

	unlock();

	if (! found) {
		throw sid_not_found(SID_LOOKUP_FAILED_ERR);
	}
}

void gsp_auth::session::session_table::remove(const sid_info &sinfo)
{
	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		/* For remove, we blank out the entry */
		if (memcmp(utable[i].sid, sinfo.get_sid(), SID_SZ) == 0) {
			utable[i].ref = 0;
			utable[i].sid[0] = '\0';
			utable[i].user_shmid = ERROR;
			utable[i].priv_shmid = ERROR;
		}
	}

	unlock();
}

int gsp_auth::session::session_table::lookup_user_shmid(const sid_info &sinfo)
{
	int shmid = ERROR;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (memcmp(utable[i].sid, sinfo.get_sid(), SID_SZ) == 0) {
			shmid = utable[i].user_shmid;
			break;
		}
	}

	unlock();
	
	if (shmid == ERROR) throw shmid_not_found(SHMID_LOOKUP_FAILED_ERR);

	return(shmid);
}

int gsp_auth::session::session_table::lookup_priv_shmid(const sid_info &sinfo)
{
	int shmid = ERROR;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (memcmp(utable[i].sid, sinfo.get_sid(), SID_SZ) == 0) {
			shmid = utable[i].priv_shmid;
			break;
		}
	}

	unlock();
	
	if (shmid == ERROR) throw shmid_not_found(SHMID_LOOKUP_FAILED_ERR);

	return(shmid);
}

unsigned int gsp_auth::session::session_table::
lookup_ref_id(const sid_info &sinfo)
{
	unsigned int ref_id = 0;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (memcmp(utable[i].sid, sinfo.get_sid(), SID_SZ) == 0) {
			ref_id = utable[i].ref;
			break;
		}
	}

	unlock();
	
	if (ref_id == 0) throw refid_not_found(REFID_LOOKUP_FAILED_ERR);

	return(ref_id);
}

std::string gsp_auth::session::session_table::lookup_sid(unsigned int ref_id)
{
	std::string sid_str;

	if (geteuid() != 0) throw app_error(NOT_AUTHORIZED_ACCESS_ERR);

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (utable[i].ref == ref_id) {
			std::ostringstream ss;
			ss.write(&(utable[i].sid[0]), SID_SZ);

			sid_str = ss.str();
			break;
		}
	}

	unlock();
	
	if (sid_str.empty()) throw sid_not_found(SID_LOOKUP_FAILED_ERR);

	return(sid_str);
}

int gsp_auth::session::session_table::get_num_sessions()
{
	int nsess = 0;

	lock();
	
	for (int i = 0; i < MAX_SESSIONS; i++) {
		/* Count non-blank entries */
		if (! ((utable[i].ref == 0) &&
		       (utable[i].sid[0] == '\0') &&
		       (utable[i].user_shmid == ERROR) &&
		       (utable[i].priv_shmid == ERROR))) {
			nsess++;
		}
	}

	unlock();

	return(nsess);
}

int gsp_auth::session::session_table::get_num_sessions_free()
{
	int nsess_free = 0;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		/* Count blank entries */
		if ((utable[i].ref == 0) &&
		    (utable[i].sid[0] == '\0') &&
		    (utable[i].user_shmid == ERROR) &&
		    (utable[i].priv_shmid == ERROR)) {
			nsess_free++;
		}
	}

	unlock();

	return(nsess_free);
}

std::map<std::string, int> gsp_auth::session::session_table::get_user_stable()
{
	std::map<std::string, int> m;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (! ((utable[i].ref == 0) &&
		       (utable[i].sid[0] == '\0') &&
		       (utable[i].user_shmid == ERROR))) {
			std::ostringstream ss;
			ss.write(&(utable[i].sid[0]), SID_SZ);

			m[ss.str()] = utable[i].user_shmid;
		}
	}

	unlock();

	return(m);
}

std::map<std::string, int> gsp_auth::session::session_table::get_priv_stable()
{
	std::map<std::string, int> m;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (! ((utable[i].ref == 0) &&
		       (utable[i].sid[0] == '\0') &&
		       (utable[i].priv_shmid == ERROR))) {
			std::ostringstream ss;
			ss.write(&(utable[i].sid[0]), SID_SZ);

			m[ss.str()] = utable[i].priv_shmid;
		}
	}

	unlock();

	return(m);
}

std::map<std::string, unsigned int>
gsp_auth::session::session_table::get_refid_stable()
{
	std::map<std::string, unsigned int> m;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (! ((utable[i].ref == 0) &&
		       (utable[i].sid[0] == '\0') &&
		       (utable[i].user_shmid == ERROR) &&
		       (utable[i].priv_shmid == ERROR))) {
			std::ostringstream ss;
			ss.write(&(utable[i].sid[0]), SID_SZ);

			m[ss.str()] = utable[i].ref;
		}
	}

	unlock();

	return(m);
}

std::vector<std::string> gsp_auth::session::session_table::get_sids()
{
	std::vector<std::string> v;

	lock();

	for (int i = 0; i < MAX_SESSIONS; i++) {
		if (! ((utable[i].ref == 0) &&
		       (utable[i].sid[0] == '\0') &&
		       (utable[i].user_shmid == ERROR) &&
		       (utable[i].priv_shmid == ERROR))) {
			std::ostringstream ss;
			ss.write(&(utable[i].sid[0]), SID_SZ);

			v.push_back(ss.str());
		}
	}

	unlock();

	return(v);
}

const int gsp_auth::session::priv_session_data::IDLE_TIMEOUT_DEFAULT = 0;
const int gsp_auth::session::priv_session_data::KILL_CNT_DEFAULT = 0;
const std::string gsp_auth::session::priv_session_data::
INVALID_CID_SZ_ERR = "Invalid CID sz; must be 16 characters";
const std::string gsp_auth::session::priv_session_data::
INVALID_AUTH_TYPE_SZ_ERR =
	"Invalid auth type string; must be less than 50 characters";

gsp_auth::session::priv_session_data &
gsp_auth::session::priv_session_data::operator=(const priv_session_data &psd)
{
	if (this != &psd) { // avoid self-assignment
		if (gspd && gspd_allocated) {
			/* Remove old resources */
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

		gspd = new gsp_session_privdata;
		gspd_allocated = true;

		/* Same as copy constructor */
		if (psd.gspd->uinfo.username == 0) {
			gspd->uinfo.username = 0;
			gspd->uinfo.username_sz = 0;
		} else {
			gspd->uinfo.username =
				strdup(psd.gspd->uinfo.username);
			gspd->uinfo.username_sz = psd.gspd->uinfo.username_sz;
		}
		
		gspd->sid = psd.gspd->sid;
		
		gspd->parent_sid = psd.gspd->parent_sid;
		
		if (psd.gspd->ginfo.group_name == 0) {
			gspd->ginfo.group_name = 0;
			gspd->ginfo.group_name_sz = 0;
		} else {
			gspd->ginfo.group_name =
				strdup(psd.gspd->ginfo.group_name);
			gspd->ginfo.group_name_sz =
				psd.gspd->ginfo.group_name_sz;
		}
		
		gspd->attr = psd.gspd->attr;

		gspd->adsap2_tok = psd.gspd->adsap2_tok;

		gspd->session_acls.auth_acls = 0;
		gspd->session_acls.cfg_acls = 0;

		/* Copy session ACLs, if necessary */
		if (psd.gspd->session_acls.auth_acls) {
			gsp_acl_session *acl = 0;
			
			while ((acl = static_cast<gsp_acl_session *>
				(libcyc_ll_iterate(psd.gspd->session_acls.auth_acls)))) {
				gsp_acl_session *nacl = new gsp_acl_session;

				*nacl = *acl;

				gspd->session_acls.auth_acls = libcyc_ll_append(gspd->session_acls.auth_acls, nacl);
			}
		}
		if (psd.gspd->session_acls.cfg_acls) {
			gsp_acl_session *acl = 0;
			
			while ((acl = static_cast<gsp_acl_session *>
				(libcyc_ll_iterate(psd.gspd->session_acls.cfg_acls)))) {
				gsp_acl_session *nacl = new gsp_acl_session;

				*nacl = *acl;

				gspd->session_acls.cfg_acls = libcyc_ll_append(gspd->session_acls.cfg_acls, nacl);
			}
		}
	}

	return(*this);
}

gsp_auth::session::priv_session_data::
priv_session_data(const priv_session_data &psd)
{
	gspd = new gsp_session_privdata;
	gspd_allocated = true;

	/* Same as copy assignment operator */
	if (psd.gspd->uinfo.username == 0) {
		gspd->uinfo.username = 0;
		gspd->uinfo.username_sz = 0;
	} else {
		gspd->uinfo.username = strdup(psd.gspd->uinfo.username);
		gspd->uinfo.username_sz = psd.gspd->uinfo.username_sz;
	}
	
	gspd->sid = psd.gspd->sid;
	
	gspd->parent_sid = psd.gspd->parent_sid;
	
	if (psd.gspd->ginfo.group_name == 0) {
			gspd->ginfo.group_name = 0;
			gspd->ginfo.group_name_sz = 0;
	} else {
		gspd->ginfo.group_name =
			strdup(psd.gspd->ginfo.group_name);
		gspd->ginfo.group_name_sz =
			psd.gspd->ginfo.group_name_sz;
	}
	
	gspd->attr = psd.gspd->attr;

	gspd->adsap2_tok = psd.gspd->adsap2_tok;

	gspd->session_acls.auth_acls = 0;
	gspd->session_acls.cfg_acls = 0;

	/* Copy session ACLs, if necessary */
	if (psd.gspd->session_acls.auth_acls) {
		gsp_acl_session *acl = 0;
		
		while ((acl = static_cast<gsp_acl_session *>
			(libcyc_ll_iterate(psd.gspd->session_acls.auth_acls)))) {
			gsp_acl_session *nacl = new gsp_acl_session;
			
			*nacl = *acl;
			
			gspd->session_acls.auth_acls = libcyc_ll_append(gspd->session_acls.auth_acls, nacl);
		}
	}
	if (psd.gspd->session_acls.cfg_acls) {
		gsp_acl_session *acl = 0;
		
		while ((acl = static_cast<gsp_acl_session *>
			(libcyc_ll_iterate(psd.gspd->session_acls.cfg_acls)))) {
			gsp_acl_session *nacl = new gsp_acl_session;
			
			*nacl = *acl;
			
			gspd->session_acls.cfg_acls = libcyc_ll_append(gspd->session_acls.cfg_acls, nacl);
		}
	}
}

void gsp_auth::session::priv_session_data::set_username(const char *s)
{
	if (gspd->uinfo.username) {
		free((void *)gspd->uinfo.username);
	}

	if (s) {
		gspd->uinfo.username_sz = strlen(s);
		gspd->uinfo.username = strdup(s);
	} else {
		gspd->uinfo.username = 0;
	}
}

void gsp_auth::session::priv_session_data::add_group_name(const char *s)
{
	/* Parse the list of group names from the session */
	gsp_auth::group::remote_group rg(gspd->ginfo.group_name);
	rg.parse();
	std::list<std::string> &rg_list = rg.get_groups();

	/* Parse the group names passed in as an argument */
	gsp_auth::group::remote_group new_rg(s);
	new_rg.parse();
	std::list<std::string> &new_rg_list = new_rg.get_groups();

	/* Combine the two lists and update the string in the obj */
	rg_list.splice(rg_list.end(), new_rg_list);

	/* Eliminate duplicates */
	rg_list.sort();
	rg_list.unique();

	rg.unparse(); // Prepare the text string from the list

	/* Update the session with the new group name string */
	free((void *)gspd->ginfo.group_name);
	gspd->ginfo.group_name = strdup(rg.get_group_name().c_str());
	gspd->ginfo.group_name_sz = strlen(gspd->ginfo.group_name);
}

void gsp_auth::session::priv_session_data::set_group_name(const char *s)
{
	if (gspd->ginfo.group_name) {
		free((void *)gspd->ginfo.group_name);
	}

	if (s) {
		gspd->ginfo.group_name_sz = strlen(s);
		gspd->ginfo.group_name = strdup(s);
	} else {
		gspd->ginfo.group_name = 0;
	}
}

void gsp_auth::session::priv_session_data::set_auth_type(std::string at)
{
	if (at.empty()) {
		gspd->attr.auth_type[0] = '\0';
		return;
	}

	strncpy(gspd->attr.auth_type, at.c_str(), GSP_AUTH_TYPE_SZ - 1);
	gspd->attr.auth_type[GSP_AUTH_TYPE_SZ - 1] = '\0'; // be sure
}

void gsp_auth::session::priv_session_data::set_curr_auth_type(std::string at)
{
	if (at.empty()) {
		gspd->attr.curr_auth_type[0] = '\0';
		return;
	}

	strncpy(gspd->attr.curr_auth_type, at.c_str(), GSP_AUTH_TYPE_SZ - 1);
	gspd->attr.curr_auth_type[GSP_AUTH_TYPE_SZ - 1] = '\0'; // be sure
}

std::string gsp_auth::session::priv_session_data::get_cid() const
{
	if (gspd->attr.cid[0] == '\0') return("");

	std::ostringstream s;
	s.write(gspd->attr.cid, GSP_CID_SZ);

	return(s.str());
}

void gsp_auth::session::priv_session_data::set_cid(std::string cid)
{
	if (cid.empty()) {
		memset(gspd->attr.cid, '\0', sizeof(gspd->attr.cid));
		return;
	}

	int size = cid.size();
	if (size != GSP_CID_SZ) throw app_error(INVALID_CID_SZ_ERR);

	memcpy(gspd->attr.cid, cid.c_str(), size);
}

std::string gsp_auth::session::priv_session_data::get_ipv4_addr() const
{
	if (gspd->attr.ipaddr_fam != GSP_AF_INET) return("");

	char ipaddr[INET_ADDRSTRLEN];

	const char *ret = inet_ntop(AF_INET, &(gspd->attr.ipaddr.ipv4),
				    &ipaddr[0], INET_ADDRSTRLEN);

	if (ret == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(ipaddr);
}

void gsp_auth::session::priv_session_data::set_ipv4_addr(std::string s)
{
	int ret = inet_pton(AF_INET, s.c_str(), &(gspd->attr.ipaddr.ipv4));
	if (ret < 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	} else if (ret == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid IP address specified";
		throw app_error(err_msg.str());
	}

	gspd->attr.ipaddr_fam = GSP_AF_INET;
}

std::string gsp_auth::session::priv_session_data::get_ipv6_addr() const
{
	if (gspd->attr.ipaddr_fam != GSP_AF_INET6) return("");

	char ipaddr[INET6_ADDRSTRLEN];

	const char *ret = inet_ntop(AF_INET6, &(gspd->attr.ipaddr.ipv6),
				    &ipaddr[0],
				    INET6_ADDRSTRLEN);

	if (ret == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(ipaddr);
}
	
void gsp_auth::session::priv_session_data::set_ipv6_addr(std::string s)
{
	int ret = inet_pton(AF_INET6, s.c_str(), &(gspd->attr.ipaddr.ipv6));
	if (ret < 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	} else if (ret == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid IP address specified";
		throw app_error(err_msg.str());
	}
	
	gspd->attr.ipaddr_fam = GSP_AF_INET6;
}

void gsp_auth::session::priv_session_data::
set_kill_username(std::string kill_username)
{
	if (kill_username.empty()) {
		gspd->attr.kill_username[0] = '\0';
		return;
	}

	strncpy(gspd->attr.kill_username, kill_username.c_str(),
		GSP_KILL_USERNAME_SZ - 1);
	gspd->attr.kill_username[GSP_KILL_USERNAME_SZ - 1] = '\0'; // be sure
}

void gsp_auth::session::priv_session_data::
set_adsap2_conn_id(std::string conn_id)
{
	if (conn_id.empty()) {
		gspd->adsap2_tok.conn_id[0] = '\0';
		return;
	}

	strncpy(gspd->adsap2_tok.conn_id, conn_id.c_str(),
		GSP_CONNECTION_ID_SZ - 1);
	gspd->adsap2_tok.conn_id[GSP_CONNECTION_ID_SZ - 1] = '\0'; // be sure
}

gsp_auth::session::user_session_data &
gsp_auth::session::user_session_data::operator=(const user_session_data &usd)
{
	if (this != &usd) { // avoid self-assignment
		if (gsd == 0) {
			gsd = new gsp_session_data;
			gsd_allocated = true;
		}
		
		/* Same as copy constructor */
		*gsd = *(usd.gsd);
	}

	return(*this);
}

gsp_auth::session::user_session_data::
user_session_data(const user_session_data &usd)
{
	gsd = new gsp_session_data;
	gsd_allocated = true;
	
	/* Same as copy assignment operator */
	*gsd = *(usd.gsd);
}

const std::string gsp_auth::session::priv_session::NULL_SESSION_ERR = "Private session structure unavailable: try calling shm_restore() before calling this function";

void gsp_auth::session::priv_session::init()
{
	/* Don't even bother to create the semaphore if we're not
	   root */
	if (geteuid() == 0) {
		// Set up the semaphores we'll need
		sem = new gsp_auth::util::
			sem(PROJECT_ID(FTOK_PRIV_SESSION_SEMS));
		sem->screate(SEM_NUM_SEMS(1), SEM_FLAG(0644));
	}
}

/* Calculate the size of the memory segment to hold our marshalled data */
size_t gsp_auth::session::priv_session::get_session_sz()
{
	if (session == 0) throw app_error(NULL_SESSION_ERR);

	size_t sz = 0;

	sz += sizeof(size_t); // Length of username
	sz++; // separator
	sz += session->get_username().size(); // Username
	sz++; // separator
	sz += sizeof(int); // Length of SID
	sz++; // separator
	sz += session->get_sid()->sid_sz; // SID
	sz++; // separator
	sz += sizeof(int); // Length of Parent SID
	sz++; // separator
	sz += session->get_parent_sid()->sid_sz; // Parent SID
	sz++; // separator
	sz += sizeof(size_t); // Length of Group name string
	sz++; // separator
	sz += session->get_group_name().size(); // Group name string
	sz++; // separator
	sz += sizeof(gsp_auth_privattr); // size of attr
	sz++; // separator
	sz += sizeof(gsp_auth_adsap2_token); // size of adsap2_tok

	/* If there are ACLs stored, determine their size */
	libcyc_list *acls;

	acls = session->get_acl_session()->auth_acls;
	if (acls) {
		sz++; // separator
		sz += sizeof(int); // Number of ACLs
		sz++; // separator
		sz += libcyc_ll_get_size(acls) * sizeof(gsp_acl_item);
	}
	acls = session->get_acl_session()->cfg_acls;
	if (acls) {
		sz++; // separator
		sz += sizeof(int); // Number of ACLs
		sz++; // separator
		sz += libcyc_ll_get_size(acls) * sizeof(gsp_acl_item);
	}

	return(sz);
}

const char gsp_auth::session::priv_session::auth_aclsep = '|';
const char gsp_auth::session::priv_session::cfg_aclsep = '-';

/* Marshal: Convert C data structure to a flat format to store
 * into shared memory */
void gsp_auth::session::priv_session::marshal(char *mdata)
{
	const size_t username_sz = session->get_username().size();
	const size_t group_name_sz = session->get_group_name().size();
	const char ws = ' ';
	const size_t sep_sz = sizeof(char);
	size_t offset = 0;

	/* Username size */
	memcpy(mdata + offset, &username_sz, sizeof(size_t));
	offset += sizeof(size_t);

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* Username */
	memcpy(mdata + offset, session->get_username().c_str(), username_sz);
	offset += username_sz;

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* SID size */
	memcpy(mdata + offset, &(session->get_sid()->sid_sz), sizeof(int));
	offset += sizeof(int);

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* SID */
	memcpy(mdata + offset, session->get_sid()->sid,
	       session->get_sid()->sid_sz);
	offset += session->get_sid()->sid_sz;

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* Parent SID size */
	memcpy(mdata + offset, &(session->get_parent_sid()->sid_sz),
	       sizeof(int));
	offset += sizeof(int);

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* Parent SID */
	memcpy(mdata + offset, session->get_parent_sid()->sid,
	       session->get_parent_sid()->sid_sz);
	offset += session->get_parent_sid()->sid_sz;

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* group_name size */
	memcpy(mdata + offset, &group_name_sz, sizeof(size_t));
	offset += sizeof(size_t);

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* group_name */
	memcpy(mdata + offset, session->get_group_name().c_str(),
	       group_name_sz);
	offset += group_name_sz;

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* Attribute structure */
	memcpy(mdata + offset, session->get_attr(), sizeof(gsp_auth_privattr));
	offset += sizeof(gsp_auth_privattr);

	/* Separator */
	memcpy(mdata + offset, &ws, sep_sz);
	offset += sep_sz;

	/* ADSAP2 token */
	memcpy(mdata + offset, session->get_adsap2_tok(),
	       sizeof(gsp_auth_adsap2_token));
	offset += sizeof(gsp_auth_adsap2_token);

	/* ACLs */
	libcyc_list *acls;

	acls = session->get_acl_session()->auth_acls;
	if (acls) {
		gsp_acl_item *acl = NULL;
		int num_acls = libcyc_ll_get_size(acls);
		
		/* Separator */
		memcpy(mdata + offset, &auth_aclsep, sep_sz);
		offset += sep_sz;

		memcpy(mdata + offset, &num_acls, sizeof(int));
		offset += sizeof(int);
		
		/* Separator */
		memcpy(mdata + offset, &ws, sep_sz);
		offset += sep_sz;
		
		while((acl = static_cast<gsp_acl_item *>
		       (libcyc_ll_iterate(acls)))) {
			memcpy(mdata + offset, acl, sizeof(gsp_acl_item));
			offset += sizeof(gsp_acl_item);
		}
	}
	acls = session->get_acl_session()->cfg_acls;
	if (acls) {
		gsp_acl_item *acl = NULL;
		int num_acls = libcyc_ll_get_size(acls);
		
		/* Separator */
		memcpy(mdata + offset, &cfg_aclsep, sep_sz);
		offset += sep_sz;

		memcpy(mdata + offset, &num_acls, sizeof(int));
		offset += sizeof(int);
		
		/* Separator */
		memcpy(mdata + offset, &ws, sep_sz);
		offset += sep_sz;
		
		while((acl = static_cast<gsp_acl_item *>
		       (libcyc_ll_iterate(acls)))) {
			memcpy(mdata + offset, acl, sizeof(gsp_acl_item));
			offset += sizeof(gsp_acl_item);
		}
	}
}

/* Unmarshal: Convert flat format in shared memory back into
 * a C data structure */
void gsp_auth::session::priv_session::unmarshal(const char *mdata, size_t sz)
{
	const size_t sep_sz = sizeof(char);
	size_t offset = 0;

	/* Read in username size */
	size_t username_sz;
	memcpy(&username_sz, mdata + offset, sizeof(size_t));
	offset += sizeof(size_t) + sep_sz; // size & separator

	/* Read in username */
	std::ostringstream uos;
	uos.write(mdata + offset, username_sz);
	session->set_username(uos.str());
	offset += username_sz + sep_sz;

	/* Read in SID size */
	memcpy(&(session->get_sid()->sid_sz), mdata + offset, sizeof(int));
	offset += sizeof(int) + sep_sz;

	/* Read in SID */
	memcpy(session->get_sid()->sid, mdata + offset,
	       session->get_sid()->sid_sz);
	offset += session->get_sid()->sid_sz + sep_sz;

	/* Read in Parent SID size */
	memcpy(&(session->get_parent_sid()->sid_sz), mdata + offset,
	       sizeof(int));
	offset += sizeof(int) + sep_sz;
	
	/* Read in Parent SID */
	memcpy(session->get_parent_sid()->sid, mdata + offset,
	       session->get_parent_sid()->sid_sz);
	offset += session->get_parent_sid()->sid_sz + sep_sz;
	
	/* Read in group_name size */
	size_t group_name_sz;
	memcpy(&group_name_sz, mdata + offset, sizeof(size_t));
	offset += sizeof(size_t) + sep_sz;
	
	/* Read in group_name */
	std::ostringstream gos;
	gos.write(mdata + offset, group_name_sz);
	session->set_group_name(gos.str());
	offset += group_name_sz + sep_sz;

	/* Read in attribute structure */
	memcpy(session->get_attr(), mdata + offset, sizeof(gsp_auth_privattr));
	offset += sizeof(gsp_auth_privattr) + sep_sz;

	/* Read in ADSAP2 token */
	memcpy(session->get_adsap2_tok(), mdata + offset,
	       sizeof(gsp_auth_adsap2_token));
	offset += sizeof(gsp_auth_adsap2_token);

	/* ACLs */
	if (offset < sz) { // Still more data
		/* Check for presence of auth ACLs */
		if (*(mdata + offset) == auth_aclsep) {
			offset += sep_sz;
			
			libcyc_list **acls =
				&(session->get_acl_session()->auth_acls);
			int num_acls = 0;
			
			memcpy(&num_acls, mdata + offset, sizeof(int));
			offset += sizeof(int) + sep_sz;
			
			for (int i = 0; i < num_acls; i++) {
				gsp_acl_item *acl = new gsp_acl_item;
				
				memcpy(acl, mdata + offset,
				       sizeof(gsp_acl_item));
				offset += sizeof(gsp_acl_item);
				
				*acls = libcyc_ll_append(*acls, acl);
			}
		}

		/* Check for presence of cfg ACLs */
		if (*(mdata + offset) == cfg_aclsep) {
			offset += sep_sz;
			
			libcyc_list **acls =
				&(session->get_acl_session()->cfg_acls);
			int num_acls = 0;
			
			memcpy(&num_acls, mdata + offset, sizeof(int));
			offset += sizeof(int) + sep_sz;
			
			for (int i = 0; i < num_acls; i++) {
				gsp_acl_item *acl = new gsp_acl_item;
				
				memcpy(acl, mdata + offset,
				       sizeof(gsp_acl_item));
				offset += sizeof(gsp_acl_item);
				
				*acls = libcyc_ll_append(*acls, acl);
			}
		}
	}
}

int gsp_auth::session::priv_session::shm_save()
{
	if (session == 0) throw app_error(NULL_SESSION_ERR);

	size_t shm_sz = get_session_sz();

	cyc_shmem us(IPC_KEY::NEW());

	us.create(SHM_SZ(shm_sz), SHM_FLAG(cyc_ipc::CYC_IPC_CREAT|0644));
	void *mem = us.attach();
	char *mdata = reinterpret_cast<char *>(mem);

	marshal(mdata);

	us.detach();

	return(us.get_id());
}

void gsp_auth::session::priv_session::shm_restore(const SHM_ID &shmid)
{
	cyc_shmem us(shmid);

	void *mem = us.attach();
	char *mdata = reinterpret_cast<char *>(mem);
	size_t shm_sz = us.get_size();

	set(new priv_session_data); /* Assign new user_session_data to
				     * instance var session */
	session_allocated = true;

	lock();
	unmarshal(mdata, shm_sz);
	unlock();

	us.detach();
}

int gsp_auth::session::priv_session::shm_update(const SHM_ID &shmid)
{
	if (session == 0) throw app_error(NULL_SESSION_ERR);
	
	int new_shmid = ERROR;

	cyc_shmem us(shmid);

	void *mem = us.attach();
	char *mdata = reinterpret_cast<char *>(mem);
	size_t new_sz = get_session_sz();
	size_t shm_sz = us.get_size();

	lock();

	/* Reuse the shared memory segment if our new data isn't
	   bigger than the old data */
	if (new_sz <= shm_sz) {
		marshal(mdata);
		new_shmid = shmid.val;

		/* Clear unused part of shared memory */
		if (new_sz < shm_sz) {
			memset(mdata + new_sz, '\0', shm_sz-new_sz);
		}
	} else {
		new_shmid = shm_save();
	}

	unlock();

	us.detach();

	return(new_shmid);
}

const std::string gsp_auth::session::user_session::NULL_SESSION_ERR = "User session structure unavailable: try calling shm_restore() before calling this function";

void gsp_auth::session::user_session::init()
{
	/* Don't even bother to create the semaphore if we're not
	   root */
	if (geteuid() == 0) {
		// Set up the semaphores we'll need
		sem = new gsp_auth::util::
			sem(PROJECT_ID(FTOK_USER_SESSION_SEMS));
		sem->screate(SEM_NUM_SEMS(1), SEM_FLAG(0644));
	}
}

int gsp_auth::session::user_session::shm_save()
{
	uid_t uid = 0; // default to root

	if (session == 0) throw app_error(NULL_SESSION_ERR);

	try {
		avo::util::password pwd(username);
		uid = pwd.get_uid();
	} catch (...) {}

	cyc_shmem us(IPC_KEY::NEW());

	us.create(SHM_SZ(sizeof(gsp_session_data)),
		  SHM_FLAG(cyc_ipc::CYC_IPC_CREAT|0644));
	us.set_uid(uid);
	void *mem = us.attach();

	gsp_session_data *gsd = new (mem) gsp_session_data;
	*gsd = *(get_session()->get_gsd()); // copy data to shared memory

	us.detach();

	return(us.get_id());
}

void gsp_auth::session::user_session::shm_restore(const SHM_ID &shmid)
{
	cyc_shmem us(shmid);

	void *mem = us.attach();

	if (session == 0) {
		set(new user_session_data); /* Assign new user_session_data to
					     * instance var session */
		session_allocated = true;
	}

	gsp_session_data *gsd = new (mem) gsp_session_data;

	lock();
	*(get_session()->get_gsd()) = *gsd; // copy data from shared memory
	unlock();

	us.detach();
}

int gsp_auth::session::user_session::shm_update(const SHM_ID &shmid)
{
	if (session == 0) throw app_error(NULL_SESSION_ERR);

	cyc_shmem us(shmid);

	void *mem = us.attach();

	gsp_session_data *gsd = new (mem) gsp_session_data;

	lock();

	*gsd = *(get_session()->get_gsd()); // copy data to shared memory

	unlock();

	us.detach();

	return(shmid.val);
}

/* Get the UID from the password database and set the UID of the
 * shared memory segment appropriately */
void gsp_auth::session::user_session::shm_update_perms(const SHM_ID &shmid)
{
	if (username.empty()) return;

	cyc_shmem us(shmid);

	avo::util::password pwd(username);
	
	us.set_uid(pwd.get_uid());
}

/* This function does the actual work; the public session create functions
 * are wrappers */
int gsp_auth::session::
priv_gsp_auth_subsession_create(const char *un,
				struct sid_data *sid,
				struct sid_data *parent_sid,
				unsigned int adsap2_handle)
{
	using namespace gsp_auth::session;
        avo::util::syslog_helper sh(gsp_auth::libinfo::get_name(),
                                    LOG_PID, LOG_USER);
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;
	uid_t x;

	if (sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sid is NULL" << std::endl;
		sh.log(LOG_ERR, err_msg.str());
		return(ERROR);
	}

	seteuid(0);

	if ((x=geteuid()) != 0) {
		std::ostringstream err_msg;
		err_msg << "Sessions may only be created by a user with err_msg root privileges: " << x << std::endl;
		sh.log(LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		int priv_shmid = ERROR;
		int user_shmid = ERROR;
		std::string username(un ? un : "");

		apih = new gsp_auth::public_api::api_helper;

		/* Make sure we're creating only one session at-a-time */
		apih->lock_create();

		/* Populate sinfo with SID */
		sid_info sinfo(sid);
		gsp_auth::util::session_id gen_sid(&sinfo);
		gen_sid.generate(username);

		/* Session table and session manager objects */
		session_table t;
		priv_session_data psd;
		user_session_data usd;
	
		/* Prepare our private session structure; inherit data from
		 * parent session and then update the fields */
		if (parent_sid && (parent_sid->sid_sz != 0)) {
			int parent_pshmid = t.lookup_priv_shmid(parent_sid);
			int parent_ushmid = t.lookup_user_shmid(parent_sid);

			priv_session parent_psess;
			parent_psess.shm_restore(SHM_ID(parent_pshmid));

			user_session parent_usess;
			parent_usess.shm_restore(SHM_ID(parent_ushmid));

			psd = *(parent_psess.get_session());
			usd = *(parent_usess.get_session());

			/* Override inheritance for certain fields */
			/* If username is specified, use it; otherwise,
			 * keep the one inherited from the parent */
			if (! username.empty()) psd.set_username(username);
			psd.update_sess_ctime();
			usd.reset_idle_cnt();
		} else {
			psd.set_username(username);
		}

		psd.set_sid(sid);
		psd.set_parent_sid(parent_sid);

		gsp_auth::util::
			update_psd(&psd,
				   gsp_auth::util::USERNAME(username),
				   gsp_auth::util::AUTH_TYPE(""),
				   gsp_auth::util::CURR_AUTH_TYPE(""));

		psd.set_adsap2_handle(adsap2_handle);

		priv_session psess(&psd);
		priv_shmid = psess.shm_save();
		
		/* Prepare our public session structure */
		usd.set_sid(sid);
		user_session usess(gsp_auth::util::USERNAME(username), &usd);
		user_shmid = usess.shm_save();

		/* Update our user table with the new entry */
		t.add(sinfo, USER_SHMID(user_shmid), PRIV_SHMID(priv_shmid));

		/* Now that the session is added and valid, update the
		 * UID of the shared-memory segment.  This is necessary
		 * when we have remote users.  With remote users, the
		 * record is acually a session, not an entry in /etc/passwd.
		 * The getpwnam called from this code must find a valid user
		 * to set the UID of the shared memory. */
		try {
			usess.shm_update_perms(SHM_ID(user_shmid));
		} catch (...) {
			t.remove(sinfo);
			gsp_auth::util::
				shm_destroy(SHM_ID(user_shmid));
			gsp_auth::util::
				shm_destroy(SHM_ID(priv_shmid));

			throw;
		}
	} catch (usess_tbl_full e) {
		/* The new session we saved must be destroyed since it
		 * couldn't be added to the session lookup table */
		gsp_auth::util::
			shm_destroy(SHM_ID(e.get_user_shmid()));
		gsp_auth::util::
			shm_destroy(SHM_ID(e.get_priv_shmid()));
		sh.log(LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}
	
	try {
		if (apih) apih->unlock_create();
	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;
	
	return(ret);
}

/* 
 * Public C API Implementation
 */
int gsp_auth_session_create(struct sid_data *sid)
{
	using namespace gsp_auth::session;

	sid_data null_psid = { "", 0 }; // Blank parent SID

	return(priv_gsp_auth_subsession_create(0, sid, &null_psid, 0));
}

int gsp_auth_subsession_create(struct sid_data *sid,
			       struct sid_data *parent_sid)
{
	using namespace gsp_auth::session;

	return(priv_gsp_auth_subsession_create(0, sid, parent_sid, 0));
}

int gsp_auth_session_create_username(const char *username,
				     struct sid_data *sid)
{
	using namespace gsp_auth::session;

	sid_data null_psid = { "", 0 }; // Blank parent SID

	return(priv_gsp_auth_subsession_create(username,
					       sid, &null_psid, 0));
}

int gsp_auth_subsession_create_username(const char *username,
					struct sid_data *sid,
					struct sid_data *parent_sid)
{
	using namespace gsp_auth::session;

	if (username == 0) {
		std::ostringstream err_msg;
		err_msg << ": username is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	return(priv_gsp_auth_subsession_create(username,
					       sid, parent_sid, 0));
}

int gsp_auth_session_create_adsap2(const char *username,
				   unsigned int adsap2_handle,
				   struct sid_data *sid)
{
	using namespace gsp_auth::session;

	sid_data null_psid = { "", 0 }; // Blank parent SID

	return(priv_gsp_auth_subsession_create(username,
					       sid, &null_psid,
					       adsap2_handle));
}

int gsp_auth_subsession_create_adsap2(const char *username,
				      unsigned int adsap2_handle,
				      struct sid_data *sid,
				      struct sid_data *parent_sid)
{
	using namespace gsp_auth::session;

	if (username == 0) {
		std::ostringstream err_msg;
		err_msg << ": username is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	return(priv_gsp_auth_subsession_create(username,
					       sid, parent_sid,
					       adsap2_handle));
}

int gsp_auth_session_destroy(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sid is NULL";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		session_table t;
		int user_shmid = t.lookup_user_shmid(sid);
		int priv_shmid = t.lookup_priv_shmid(sid);

		t.remove(sid);
		gsp_auth::util::shm_destroy(SHM_ID(user_shmid)); 
		gsp_auth::util::shm_destroy(SHM_ID(priv_shmid)); 

		/* Clear SID */
		std::istringstream is("");
		sid_info sinfo(sid);
		is >> sinfo;
	} catch (shmid_not_found e) {
		ret = ERROR;
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_destroy_ref_id(unsigned int ref_id)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (ref_id == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid reference ID";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_data sdata;
		sid_info sinfo(&sdata);
		session_table t;
		std::string sid_str = t.lookup_sid(ref_id);

		if (sid_str.empty()) {
			ret = ERROR;
		} else {
			std::istringstream is(sid_str);
			is >> sinfo;

			int user_shmid = t.lookup_user_shmid(sinfo);
			int priv_shmid = t.lookup_priv_shmid(sinfo);
			
			t.remove(sinfo);
			gsp_auth::util::shm_destroy(SHM_ID(user_shmid)); 
			gsp_auth::util::shm_destroy(SHM_ID(priv_shmid)); 
		}
	} catch (refid_not_found e) {
		ret = ERROR;
	} catch (shmid_not_found e) {
		ret = ERROR;
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_num_sessions()
{
	using namespace gsp_auth::session;
	int nsess = ERROR;

	try {
		session_table t;
		nsess = t.get_num_sessions();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(nsess);
}

int gsp_auth_session_get_num_sessions_free()
{
	using namespace gsp_auth::session;
	int nsess_free = ERROR;

	try {
		session_table t;
		nsess_free = t.get_num_sessions_free();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(nsess_free);
}

char *gsp_auth_session_get_username(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *username = 0;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		if (! psess.get_session()->get_username().empty()) {
			username = strdup(psess.get_session()->get_username().c_str());
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(username);
}

int gsp_auth_session_set_username(struct sid_data *sid,
				  const char *username)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (username == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": username not specified";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_username(username);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_sid(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *sid_str = 0;

	try {
		std::string sid_ccstr = sid_info(sid).get_sid_str();
		if (! sid_ccstr.empty()) {
			sid_str = strdup(sid_ccstr.c_str());
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(sid_str);
}

char *gsp_auth_session_get_parent_sid(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *sid_str = 0;

	try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));
               sid_data *psid = psess.get_session()->get_parent_sid();

	       std::string sid_ccstr = sid_info(psid).get_sid_str();

		if (! sid_ccstr.empty()) {
			sid_str = strdup(sid_ccstr.c_str());
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(sid_str);
}

int gsp_auth_session_get_parent_sdata(struct sid_data *sid,
				      struct sid_data *parent_sid)
{
	using namespace gsp_auth::session;
	int ret = ERROR;

	try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       priv_session_data *psd = psess.get_session();

	       if (psd->has_parent()) {
		       *parent_sid = *(psd->get_parent_sid());
			ret = 0;
	       }
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(ret);
}

int gsp_auth_session_set_parent_sdata(struct sid_data *sid,
				      struct sid_data *parent_sid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (parent_sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": parent SID not specified";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_parent_sid(parent_sid);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t, PRIV_SHMID(priv_shmid), psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_group_name(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *group_name = 0;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		if (! psess.get_session()->get_group_name().empty()) {
			group_name = strdup(psess.get_session()->get_group_name().c_str());
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(group_name);
}

int gsp_auth_session_set_group_name(struct sid_data *sid,
				    const char *group_name)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (group_name == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": group name not specified";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_group_name(group_name);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

/* Like gsp_auth_session_set_group_name() except appends instead of
 * overwrites. */
int gsp_auth_session_add_group_name(struct sid_data *sid,
				    const char *group_name)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (group_name == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": group name not specified";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		/* Grab session and session manager */
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		priv_session_data *psd = psess.get_session();

		if (psd->get_group_name().empty()) {
			psd->set_group_name(group_name);
		} else {
			psd->add_group_name(group_name);
		}

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_attr_flag(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int flag = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		flag = psess.get_session()->get_flags();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(flag);
}

int gsp_auth_session_add_attr_flag(struct sid_data *sid, int flag)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->add_flag(flag);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_del_attr_flag(struct sid_data *sid, int flag)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->del_flag(flag);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

enum sid_status gsp_auth_session_sid_check(const char *sid_str)
{
	using namespace gsp_auth::session;

	if (sid_str == 0) return(GSP_AUTH_SID_NULL);
	if (strlen(sid_str) > SID_SZ) return(GSP_AUTH_SID_ERANGE);

	try {
		sid_info sinfo;
		std::istringstream is(sid_str);
		is >> sinfo;

		/* If the lookup doesn't thrown exception, the session is
		 * valid (i.e. the lookup succeeded) */
		session_table t;
		t.lookup_user_shmid(sinfo);
	} catch (shmid_not_found e) {
		return(GSP_AUTH_SID_EINVAL);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(GSP_AUTH_SID_ERR);
	} catch (...) {
		return(GSP_AUTH_SID_ERR);
	}

	return(GSP_AUTH_SID_VALID);
}

enum sid_status gsp_auth_session_sdata_check(sid_data *sid)
{
	using namespace gsp_auth::session;

	if (sid == 0) return(GSP_AUTH_SID_NULL);

	try {
		/* If the lookup doesn't thrown exception, the session is
		 * valid (i.e. the lookup succeeded) */
		session_table t;
		t.lookup_user_shmid(sid);
	} catch (shmid_not_found e) {
		return(GSP_AUTH_SID_EINVAL);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(GSP_AUTH_SID_ERR);
	} catch (...) {
		return(GSP_AUTH_SID_ERR);
	}

	return(GSP_AUTH_SID_VALID);
}

const char *gsp_auth_session_sid_strerror(enum sid_status stat)
{
	static const char * const sid_err_str = /* GSP_AUTH_SID_ERR */
		"Error in determining the status of the specified SID";

	static const char * const sid_status_strs[] = {
		/* GSP_AUTH_SID_VALID */
		"SID valid",
		/* GSP_AUTH_SID_EINVAL */
		"SID invalid",
		/* GSP_AUTH_SID_ERANGE */
		"SID too long",
		/* GSP_AUTH_SID_NULL */
		"SID null",
	};

	if (stat == GSP_AUTH_SID_ERR) return(sid_err_str);
	
	return(sid_status_strs[stat]);
}

/* Populate the sid_data structure passed in as an argument with
 * the text string passed in as an argument if the session actually
 * exists.  This function is useful if we have a sid string and
 * want to obtain a valid sid_data structure for use in the
 * Authentication API. */
enum sid_status gsp_auth_session_sid_data(const char *sid_str,
					  struct sid_data *sid)
{
	using namespace gsp_auth::session;
	enum sid_status ret = GSP_AUTH_SID_VALID;
	std::istringstream blank_is("");

	try {
		sid_info sinfo(sid);

		std::istringstream is(sid_str);
		is >> sinfo;

		/* The idea here is that a failed lookup will throw
		 * an exception; any failure will "blank" or "erase"
		 * the sid_data structure that the user passed in */
		session_table t;
		t.lookup_user_shmid(sinfo);
	} catch (shmid_not_found e) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		ret = GSP_AUTH_SID_EINVAL;
	} catch (app_error e) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = GSP_AUTH_SID_ERR;
	} catch (...) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		ret = GSP_AUTH_SID_ERR;
	}

	return(ret);
}

char *gsp_auth_session_get_auth_type(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       char *auth_type = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       if (! psess.get_session()->get_auth_type().empty()) {
		       auth_type = strdup(psess.get_session()->get_auth_type().c_str());
	       }
       } catch (shmid_not_found e) {
               return(0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(0);
       } catch (...) {
               return(0);
       }

       return(auth_type);
}

int gsp_auth_session_set_auth_type(struct sid_data *sid, const char *auth_type)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_auth_type(auth_type);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_curr_auth_type(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       char *auth_type = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       if (! psess.get_session()->get_curr_auth_type().empty()) {
		       auth_type = strdup(psess.get_session()->get_curr_auth_type().c_str());
	       }
       } catch (shmid_not_found e) {
               return(0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(0);
       } catch (...) {
               return(0);
       }

       return(auth_type);
}

int gsp_auth_session_set_curr_auth_type(struct sid_data *sid,
					const char *auth_type)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_curr_auth_type(auth_type);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_ip_addr(struct sid_data *sid,
				   enum gsp_inet_family *inet_fam)
{
	using namespace gsp_auth::session;
	char *ipaddr = 0;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));

		*inet_fam = psess.get_session()->get_ipaddr_fam();
		switch (*inet_fam) {
		case GSP_AF_INET:
			ipaddr = strdup(psess.get_session()->get_ipv4_addr().c_str());
			break;
		case GSP_AF_INET6:
			ipaddr = strdup(psess.get_session()->get_ipv6_addr().c_str());
			break;
		case GSP_AF_UNKNOWN:
		default:
			ipaddr = 0;
		}
	} catch (shmid_not_found e) {
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(ipaddr);
}

int gsp_auth_session_set_ip_addr(struct sid_data *sid, const char *ipaddr)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (ipaddr == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": ipaddr is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		/* Figure out if we've got an IPv4 address by trying
		 * to parse the octets */
		int ipoct[4];
		int nitem = sscanf(ipaddr, "%d.%d.%d.%d",
				   &ipoct[3], &ipoct[2], &ipoct[1], &ipoct[0]);
		if (nitem == 4) {
			psess.get_session()->set_ipv4_addr(ipaddr);
		} else {
			psess.get_session()->set_ipv6_addr(ipaddr);
		}

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_ipv4_addr(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *ipaddr = 0;
	enum gsp_inet_family inet_fam = GSP_AF_UNKNOWN;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));

		inet_fam = psess.get_session()->get_ipaddr_fam();
		switch (inet_fam) {
		case GSP_AF_INET:
			ipaddr = strdup(psess.get_session()->get_ipv4_addr().c_str());
			break;
		case GSP_AF_INET6:
		case GSP_AF_UNKNOWN:
		default:
			ipaddr = 0;
		}
	} catch (shmid_not_found e) {
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(ipaddr);
}

char *gsp_auth_session_get_ipv6_addr(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	char *ipaddr = 0;
	enum gsp_inet_family inet_fam = GSP_AF_UNKNOWN;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));

		inet_fam = psess.get_session()->get_ipaddr_fam();
		switch (inet_fam) {
		case GSP_AF_INET6:
			ipaddr = strdup(psess.get_session()->get_ipv6_addr().c_str());
			break;
		case GSP_AF_INET:
		case GSP_AF_UNKNOWN:
		default:
			ipaddr = 0;
		}
	} catch (shmid_not_found e) {
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(ipaddr);
}

int gsp_auth_session_set_ipv4_addr(struct sid_data *sid, const char *ipaddr)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (ipaddr == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": ipaddr is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_ipv4_addr(ipaddr);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_set_ipv6_addr(struct sid_data *sid, const char *ipaddr)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (ipaddr == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": ipaddr is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_ipv6_addr(ipaddr);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

pid_t gsp_auth_session_get_mpid(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	pid_t mpid = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		mpid = psess.get_session()->get_mpid();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(mpid);
}

int gsp_auth_session_set_mpid(struct sid_data *sid, pid_t pid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (pid == ERROR) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid PID" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_NOTICE, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_mpid(pid);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_radius_service_type(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int sType = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		sType = psess.get_session()->get_radius_service_type();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(sType);
}

int gsp_auth_session_set_radius_service_type(struct sid_data *sid, int sType)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (sType < 0 || sType > 6) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid radius service type: " << sType << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_radius_service_type(sType);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_serial_authorization(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int st = 0;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		st = psess.get_session()->get_serial_authorization();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(st);
}

int gsp_auth_session_set_serial_authorization(struct sid_data *sid, int st)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_serial_authorization(st);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_outlet_authorization(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int st = 1;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		st = psess.get_session()->get_outlet_authorization();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(st);
}

int gsp_auth_session_set_outlet_authorization(struct sid_data *sid, int st)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_outlet_authorization(st);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_tacacs_level(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int level = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		level = psess.get_session()->get_tacacs_level();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(level);
}

int gsp_auth_session_set_tacacs_level(struct sid_data *sid, int level)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (level < 0 || level > 15) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid tacacs level: " << level << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_tacacs_level(level);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_idle_to(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int idle_to = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		idle_to = psess.get_session()->get_idle_to();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(idle_to);
}

int gsp_auth_session_set_idle_to(struct sid_data *sid, int idle_to)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (idle_to < 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid idle to" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_idle_to(idle_to);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_kill_cnt(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	int kill_cnt = ERROR;

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);
		
		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));
		
		kill_cnt = psess.get_session()->get_kill_cnt();
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}
	
	return(kill_cnt);
}

int gsp_auth_session_set_kill_cnt(struct sid_data *sid, int kill_cnt)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (kill_cnt < 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": invalid kill counter value";
		err_msg << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_kill_cnt(kill_cnt);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_et_secs(sid_data *sid)
{
       using namespace gsp_auth::session;
       int et_secs = ERROR;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

               et_secs = time(0) - psess.get_session()->get_sess_ctime();
       } catch (shmid_not_found e) {
               return(ERROR);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(ERROR);
       } catch (...) {
               return(ERROR);
       }

       return(et_secs);
}

time_t gsp_auth_session_get_ctime(sid_data *sid)
{
       using namespace gsp_auth::session;
       time_t sess_ctime = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

               sess_ctime = psess.get_session()->get_sess_ctime();
       } catch (shmid_not_found e) {
               return(ERROR);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(ERROR);
       } catch (...) {
               return(ERROR);
       }

       return(sess_ctime);
}


enum gsp_auth_session_type gsp_auth_session_get_type(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       enum gsp_auth_session_type type = GSP_AUTH_SESS_UNKNOWN;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

               type = psess.get_session()->get_session_type();
       } catch (shmid_not_found e) {
               return(GSP_AUTH_SESS_UNKNOWN);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(GSP_AUTH_SESS_UNKNOWN);
       } catch (...) {
               return(GSP_AUTH_SESS_UNKNOWN);
       }

       return(type);
}

int gsp_auth_session_set_type(struct sid_data *sid,
			      enum gsp_auth_session_type type)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_session_type(type);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

enum gsp_auth_target_session_type
gsp_auth_session_get_target_type(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       enum gsp_auth_target_session_type type = GSP_AUTH_TARG_UNKNOWN;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       type = psess.get_session()->get_target_session_type();
       } catch (shmid_not_found e) {
               return(GSP_AUTH_TARG_UNKNOWN);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(GSP_AUTH_TARG_UNKNOWN);
       } catch (...) {
               return(GSP_AUTH_TARG_UNKNOWN);
       }

       return(type);
}

int gsp_auth_session_set_target_type(struct sid_data *sid,
				     enum gsp_auth_target_session_type type)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_target_session_type(type);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_target_id(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       int tid = ERROR;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       tid = psess.get_session()->get_target_id();
       } catch (shmid_not_found e) {
               return(ERROR);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(ERROR);
       } catch (...) {
               return(ERROR);
       }

       return(tid);
}

int gsp_auth_session_set_target_id(struct sid_data *sid, int tid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_target_id(tid);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_cid(struct sid_data *sid, char *cid)
{
       using namespace gsp_auth::session;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       memcpy(cid, psess.get_session()->get_cid().c_str(), GSP_CID_SZ);
       } catch (shmid_not_found e) {
               return(ERROR);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(ERROR);
       } catch (...) {
               return(ERROR);
       }

       return(0);
}

int gsp_auth_session_set_cid(struct sid_data *sid, const char *cid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		std::ostringstream s;
		s.write(cid, GSP_CID_SZ);
		psess.get_session()->set_cid(s.str());

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

int gsp_auth_session_get_adsap2_handle_from_privdata(struct gsp_session_privdata *gspd)
{
	if (gspd) return gspd->adsap2_tok.handle;
	return -1;
}

int gsp_auth_session_get_adsap2_handle(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       int ret = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       ret = psess.get_session()->get_adsap2_handle();
       } catch (shmid_not_found e) {
               return(0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(0);
       } catch (...) {
               return(0);
       }

       return(ret);
}

int gsp_auth_session_set_adsap2_handle(struct sid_data *sid,
				       unsigned int handle)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_adsap2_handle(handle);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_adsap2_conn_id(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       char *ret = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       if (! psess.get_session()->get_adsap2_conn_id().empty()) {
	               ret = strdup(psess.get_session()->get_adsap2_conn_id().c_str());
	       }
       } catch (shmid_not_found e) {
               return(0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(0);
       } catch (...) {
               return(0);
       }

       return(ret);
}

int gsp_auth_session_set_adsap2_conn_id(struct sid_data *sid,
					const char *conn_id)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_adsap2_conn_id(conn_id);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

char *gsp_auth_session_get_kill_username(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       char *ret = 0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       if (! psess.get_session()->get_kill_username().empty()) {
	               ret = strdup(psess.get_session()->get_kill_username().c_str());
	       }
       } catch (shmid_not_found e) {
               return(0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(0);
       } catch (...) {
               return(0);
       }

       return(ret);
}

int gsp_auth_session_set_kill_username(struct sid_data *sid,
				       const char *kill_username)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_kill_username(kill_username);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

uid_t gsp_auth_session_get_uid(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       uid_t ret = ~0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       ret = psess.get_session()->get_uid();
       } catch (shmid_not_found e) {
               return(~0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(~0);
       } catch (...) {
               return(~0);
       }

       return(ret);
}

int gsp_auth_session_set_uid(struct sid_data *sid, uid_t uid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_uid(uid);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

gid_t gsp_auth_session_get_gid(struct sid_data *sid)
{
       using namespace gsp_auth::session;
       gid_t ret = ~0;

       try {
               session_table t;
               int shmid = t.lookup_priv_shmid(sid);

               priv_session psess;
               psess.shm_restore(SHM_ID(shmid));

	       ret = psess.get_session()->get_gid();
       } catch (shmid_not_found e) {
               return(~0);
       } catch (app_error e) {
               avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					     LOG_ERR, e.get_str());
               return(~0);
       } catch (...) {
               return(~0);
       }

       return(ret);
}

int gsp_auth_session_set_gid(struct sid_data *sid, gid_t gid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		// Grab session and session manager
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		psess.get_session()->set_gid(gid);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

/* Add an ACL to a session */
int gsp_auth_session_acl_add(struct sid_data *sid,
			     const struct gsp_acl_item *acl)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	try {
		apih = new gsp_auth::public_api::api_helper;

		apih->lock();

		sid_info sinfo(sid);

		session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);

		/* Grab session and session manager */
		priv_session psess;
		psess.shm_restore(SHM_ID(priv_shmid));

		gsp_acl_session *gspas =
			psess.get_session()->get_acl_session();

		gsp_acl_item *aclitem = new gsp_acl_item;
		*aclitem = *acl;

		gspas->auth_acls = libcyc_ll_append(gspas->auth_acls, aclitem);

		/* Update shm with new data */
		gsp_auth::util::sess_update_helper(sinfo, t,
						   PRIV_SHMID(priv_shmid),
						   psess);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	try {
		if (apih) apih->unlock();
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

	delete apih;
	apih = 0;

	return(ret);
}

/* Functions providing direct access to shared memory */
int gsp_auth_session_idle_cnt_attach(struct sid_data *sid, int **idle_cnt)
{
	using namespace gsp_auth::session;

	if (idle_cnt == 0) return(ERROR);

	try {
		session_table t;
		int shmid = t.lookup_user_shmid(sid);

		cyc_shmem us(shmid);

		void *mem = us.attach(SHM_ADDR(0), SHM_FLAG(0)); // attach RW
		gsp_session_data *gsd = new (mem) gsp_session_data;

		*idle_cnt = &(gsd->attr.idle_cnt);
	} catch (shmid_not_found e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(0);
}

int gsp_auth_session_idle_cnt_detach(struct sid_data *sid, void *idle_addr)
{
	using namespace gsp_auth::session;

	if (idle_addr == 0) return(ERROR);

	try {
		session_table t;
		int shmid = t.lookup_user_shmid(sid);

		cyc_shmem us(shmid);

		us.detach(SHM_ADDR(idle_addr));
	} catch (shmid_not_found e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(0);
}

/* We return 0 upon error (e.g. ref ID not found) or the reference ID
 * upon success */
unsigned int gsp_auth_session_get_ref_id(struct sid_data *sid)
{
	using namespace gsp_auth::session;
	unsigned int refid = 0;

	try {
		session_table t;
		refid = t.lookup_ref_id(sid);
	} catch (refid_not_found e) {
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}

	return(refid);
}

/* We populate the data pointed to by the second argument, struct sid_data,
 * with the SID for a given reference ID passed in as the first argument;
 * ERROR is returned on error (e.g. reference ID not found) */
enum sid_status gsp_auth_session_get_sid_from_ref_id(unsigned int refid,
						     struct sid_data *sid)
{
	using namespace gsp_auth::session;
	std::istringstream blank_is("");
	enum sid_status ret = GSP_AUTH_SID_VALID;

	try {
		/* The idea here is that a failed lookup will throw
		 * an exception; any failure will "blank" or "erase"
		 * the sid_data structure that the user passed in */
		session_table t;
		std::string sid_str = t.lookup_sid(refid);

		if (sid_str.empty()) {
			return(GSP_AUTH_SID_ERR);
		} else {
			sid_info sinfo(sid);

			std::istringstream is(sid_str);
			is >> sinfo;
		}
	} catch (sid_not_found e) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		ret = GSP_AUTH_SID_EINVAL;
	} catch (app_error e) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = GSP_AUTH_SID_ERR;
	} catch (...) {
		sid_info sinfo(sid);
		blank_is >> sinfo;
		ret = GSP_AUTH_SID_ERR;
	}

	return(ret);
}

/* Build a linked list with all sessions.  The linked list is populated
 * with data items of type "struct gsp_sess_ident". */
int gsp_auth_session_get_session_list(libcyc_list **slist)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (slist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": slist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();
		std::map<std::string, unsigned int>
			rtable = t.get_refid_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to the session data */
			priv_session_data *psd = psess.get_session();

			/* Copy SID from session data to new memory and
			 * attach it to the linked list */
			gsp_sess_ident *tmp_sident =
				(gsp_sess_ident *)
				malloc(sizeof(gsp_sess_ident));
			tmp_sident->sdata = *(psd->get_sid());
			tmp_sident->ref = rtable[mi->first];
			*slist = libcyc_ll_append(*slist, tmp_sident);
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

/* This function is like gsp_auth_session_get_username_list() except that
 * it only returns a list of sessions with usernames */
int gsp_auth_session_get_sess_username_list(libcyc_list **slist)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (slist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": slist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();
		std::map<std::string, unsigned int>
			rtable = t.get_refid_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to the session data */
			priv_session_data *psd = psess.get_session();

			/* We only want sessions with usernames */
			if (psd->get_username().empty()) continue;

			/* Copy SID from session data to new memory and
			 * attach it to the linked list */
			gsp_sess_ident *tmp_sident =
				(gsp_sess_ident *)
				malloc(sizeof(gsp_sess_ident));
			tmp_sident->sdata = *(psd->get_sid());
			tmp_sident->ref = rtable[mi->first];
			*slist = libcyc_ll_append(*slist, tmp_sident);
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

/* Frees all memory allocated by gsp_auth_session_get_session_list()
 * or gsp_auth_session_get_sess_username_list() */
void gsp_auth_session_free_session_list(libcyc_list **slist)
{
	if (slist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": slist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return;
	}

	gsp_sess_ident *sident = 0;

	libcyc_ll_reset(*slist);

	while((sident =
	       static_cast<gsp_sess_ident *>(libcyc_ll_iterate(*slist)))) {
		free(sident);
	}

	libcyc_ll_free(*slist);
	*slist = 0;
}

/* Get a list of child sessions associated with a parent session */
int gsp_auth_session_get_children_list(sid_data *sid, libcyc_list **clist)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (clist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": clist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		sid_info sinfo(sid);

		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();
		std::map<std::string, unsigned int>
			rtable = t.get_refid_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to our session data object;
			 * create sid_info object for parent */
			priv_session_data *psd = psess.get_session();
			sid_info psinfo(psd->get_parent_sid());

			/* If this session instance specifies the parent
			   we're looking for, add it to the linked list */
			if (sinfo.get_sid_str() == psinfo.get_sid_str()) {
				gsp_sess_ident *tmp_sident =
					(gsp_sess_ident *)
					malloc(sizeof(gsp_sess_ident));
				tmp_sident->sdata = *(psd->get_sid());
				tmp_sident->ref = rtable[mi->first];
				*clist = libcyc_ll_append(*clist, tmp_sident);
			}
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

/* Free/deallocate all data structures associated with the list of
 * child sessions */
void gsp_auth_session_free_children_list(libcyc_list **clist)
{
	if (clist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": clist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return;
	}

	gsp_sess_ident *sident = 0;

	libcyc_ll_reset(*clist);

	while((sident =
	       static_cast<gsp_sess_ident *>(libcyc_ll_iterate(*clist)))) {
		free(sident);
	}

	libcyc_ll_free(*clist);
	*clist = 0;
}

int gsp_auth_session_get_data_user(struct sid_data *sid,
				   struct gsp_session_data *gsd)
{
	using namespace gsp_auth::session;

	try {
		session_table t;
		int shmid = t.lookup_user_shmid(sid);

		user_session usess;
		usess.shm_restore(SHM_ID(shmid));

		*gsd = *(usess.get_session()->get_gsd());
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(0);
}

void gsp_auth_session_free_data_user(struct gsp_session_data *gsd)
{
	/* Nothing to do for now; maybe a future library version
	 * will need to free something here */
}

int gsp_auth_session_get_data_priv(struct sid_data *sid,
				   struct gsp_session_privdata *gspd)
{
	using namespace gsp_auth::session;

	/* Clear some fields before start so gsp_auth_session_free_data_priv()
	 * won't segfault if it is called and we error without populating
	 * the structure */
	gspd->uinfo.username = 0;
	gspd->uinfo.username_sz = 0;
	gspd->ginfo.group_name = 0;
	gspd->ginfo.group_name_sz = 0;

	if (gspd == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": gspd is NULL";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		session_table t;
		int shmid = t.lookup_priv_shmid(sid);

		priv_session psess;
		psess.shm_restore(SHM_ID(shmid));

		priv_session_data *psd = psess.get_session();
		*gspd = *(psd->get_gspd()); // Shallow copy

		/* We're in the C domain now so we'll do the deep copy
		 * of username and group_name manually */
		if (psd->get_username().empty()) {
			gspd->uinfo.username = 0;
			gspd->uinfo.username_sz = 0;
		} else {
			gspd->uinfo.username = strdup(psd->get_username().c_str());
			gspd->uinfo.username_sz = psd->get_username().size();
		}

		if (psd->get_group_name().empty()) {
			gspd->ginfo.group_name = 0;
			gspd->ginfo.group_name_sz = 0;
		} else {
			gspd->ginfo.group_name =
				strdup(psd->get_group_name().c_str());
			gspd->ginfo.group_name_sz = psd->get_group_name().size();
		}
	} catch (shmid_not_found e) {
		return(ERROR);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(ERROR);
	} catch (...) {
		return(ERROR);
	}

	return(0);
}

/* Free the dynamic memory we allocated in the gsp_session_privdata struct */
void gsp_auth_session_free_data_priv(struct gsp_session_privdata *gspd)
{
	if (gspd == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": gspd is NULL";
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return;
	}

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
}

int gsp_auth_session_get_data_user_list(libcyc_list **sdlist)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (sdlist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sdlist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_user_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			user_session usess;
			usess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to our session data object */
			user_session_data *usd = usess.get_session();

			/* Copy the session data to new memory and
			 * attach it to the linked list */
			gsp_session_data *tmp_gsd =
				(gsp_session_data *)
				malloc(sizeof(gsp_session_data));
			*tmp_gsd = *(usd->get_gsd());
			*sdlist = libcyc_ll_append(*sdlist, tmp_gsd);
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

void gsp_auth_session_free_data_user_list(libcyc_list **sdlist)
{
	if (sdlist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sdlist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return;
	}

	gsp_session_data *gsd = 0;

	libcyc_ll_reset(*sdlist);

	while((gsd = static_cast<gsp_session_data *>
	       (libcyc_ll_iterate(*sdlist)))) {
		free(gsd);
	}

	libcyc_ll_free(*sdlist);
	*sdlist = 0;
}

int gsp_auth_session_get_data_priv_list(libcyc_list **sdlist)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = 0;

	if (sdlist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sdlist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to our session data object*/
			priv_session_data *psd = psess.get_session();

			/* Copy the session data to new memory and
			 * attach it to the linked list */
			gsp_session_privdata *tmp_gspd =
				(gsp_session_privdata *)
				malloc(sizeof(gsp_session_privdata));
			*tmp_gspd = *(psd->get_gspd());

			/* We're in the C domain now so we'll do the deep copy
			 * of username and group_name manually */
			tmp_gspd->uinfo.username =
				strdup(psd->get_username().c_str());
			tmp_gspd->uinfo.username_sz =
				psd->get_username().size();
			tmp_gspd->ginfo.group_name =
				strdup(psd->get_group_name().c_str());
			tmp_gspd->ginfo.group_name_sz =
				psd->get_group_name().size();

			*sdlist = libcyc_ll_append(*sdlist, tmp_gspd);
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

void gsp_auth_session_free_data_priv_list(libcyc_list **sdlist)
{
	if (sdlist == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sdlist is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return;
	}

	gsp_session_privdata *gspd = 0;

	while((gspd = static_cast<gsp_session_privdata *>
	       (libcyc_ll_iterate(*sdlist)))) {
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
		
		free(gspd);
	}
	
	libcyc_ll_free(*sdlist);
	*sdlist = 0;
}

int gsp_auth_session_get_sid_from_target_id(struct sid_data *parent_sid,
					    int target_id,
					    struct sid_data *sid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = ERROR;

	if (parent_sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": parent_sid is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();

		sid_info psid(parent_sid);

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to our session data object*/
			priv_session_data *psd = psess.get_session();

			/* Does our session contain the parent we're
			 * looking for?  If not, keep looking. */
			sid_info tpsid(psd->get_parent_sid());
			if (psid != tpsid) continue;

			/* We have a parent match; set the SID if the
			 * target ID matches */
			if (psd->get_target_id() == target_id) {
				sid_info sinfo(sid);

				std::istringstream is(mi->first);
				is >> sinfo;
				
				ret = 0;
				break;
			}
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

/* This function checks if connection_id is in use 
 * it returns 1 if in use */
int gsp_auth_session_connid_inuse(int connid)
{
	using namespace gsp_auth::session;
	gsp_auth::public_api::api_helper *apih = 0;
	int ret = ERROR;

	try {
		apih = new gsp_auth::public_api::api_helper;

                apih->lock();

		session_table t;
		std::map<std::string, int> utable = t.get_priv_stable();

		typedef std::map<std::string, int>::const_iterator I;
		for (I mi = utable.begin(); mi != utable.end(); ++mi) {
			/* Grab session and session manager */
			priv_session psess;
			psess.shm_restore(SHM_ID(mi->second));
			
			/* Get a pointer to our session data object*/
			priv_session_data *psd = psess.get_session();

			if (psd->get_target_id() == connid) {
				ret = 1;
				break;
			}
		}
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		ret = ERROR;
	} catch (...) {
		ret = ERROR;
	}

        try {
		if (apih) apih->unlock();
        } catch (app_error e) {
                avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
                ret = ERROR;
        } catch (...) {
                ret = ERROR;
        }

	delete apih;
	apih = 0;

	return(ret);
}

/* End Public C API Implementation */
