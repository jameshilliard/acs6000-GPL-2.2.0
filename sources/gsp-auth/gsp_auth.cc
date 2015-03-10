/*****************************************************************
 * File: libgsp_auth.c
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: libgsp_auth.c
 *
 ****************************************************************/

#include <cerrno>
#include <climits>
#include <cstring>

#include <string>
#include <sstream>

#include <unistd.h>
#include <sys/types.h>

#include <security/pam_appl.h>

#include <gsp_auth.h>
#include "auth_priv.h"

const std::string gsp_auth::libinfo::LIB_NAME = "gsp_auth";
const std::string gsp_auth::libinfo::LIB_VERSION = "0.0";
const std::string gsp_auth::libinfo::LIB_LICENSE = "Avocent";

/* Public C API Implementation */
int gsp_auth_pam_authenticate(const char *service,
			      const char *tty_name,
			      const struct pam_conv *conv,
			      pam_handle_t **pamh,
			      sid_data *sid)
{
	return(gsp_auth_pam_authenticate_username(0, service, tty_name,
						  conv, pamh, sid));
}

int gsp_auth_pam_authenticate_username(const char *username,
				       const char *service,
				       const char *tty_name,
				       const struct pam_conv *conv,
				       pam_handle_t **pamh,
				       sid_data *sid)
{
	using namespace gsp_auth::pam;
	int ret = PAM_SUCCESS;
	pam_helper *ph = 0;
	avo::util::syslog_helper sh(gsp_auth::libinfo::get_name(),
				    LOG_PID, LOG_AUTH);

	if (pamh == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": pamh is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(PAM_SYSTEM_ERR);
	}

	if (sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sid is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(PAM_SYSTEM_ERR);
	}

	/* Handle PAM init details */
	try {
		ph = new pam_helper(PAM_SERVICE_NAME(service ? service : ""),
				    PAM_USERNAME(username ? username : ""),
				    PAM_TTY_NAME(tty_name ? tty_name : ""),
				    PAM_CONVERSATION(conv));

		/* Set username */
		avo::util::password pw(getuid());
		ph->set_ruser(PAM_RUSER_NAME(pw.get_name()));

		/* Set hostname */
		char hname[HOST_NAME_MAX + 1];
		if (gethostname(hname, HOST_NAME_MAX) == -1) {
			std::ostringstream err_msg;
			err_msg << __func__ << ": " << strerror(errno);
			sh.log(LOG_ERR, err_msg.str());
			ret = PAM_SYSTEM_ERR;
		}
		hname[HOST_NAME_MAX] = '\0';
		ph->set_rhost(PAM_RHOST_NAME(hname));

		ph->authenticate(PAM_FLAGS(PAM_DISALLOW_NULL_AUTHTOK));
		ph->acct_mgmt();
		ph->setcred(PAM_FLAGS(PAM_ESTABLISH_CRED));
		ph->open_session();
	} catch (pam_error e) {
		ret = e.get_ret();
 	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = PAM_SYSTEM_ERR;
	} catch (...) {
		ret = PAM_SYSTEM_ERR;
	}

	/* Update SID with SID from PAM handle */
	try {
		ph->get_sid(sid);
	} catch (...) {} // Next block will catch an invalid SID

	/* Handle session from PAM handle.  We update the session if
	 * authentication succeeded and we destroy the session if it
	 * failed */
	try {			
		gsp_auth::session::sid_info sinfo(sid);
		
		/* Update session */
		gsp_auth::session::session_table t;
		int priv_shmid = t.lookup_priv_shmid(sinfo);
		int user_shmid = t.lookup_user_shmid(sinfo);
		
		if (ret == PAM_SUCCESS) {
			/* Get session manager and load in data */
			gsp_auth::session::priv_session psess;
			psess.shm_restore(SHM_ID(priv_shmid));
			
			gsp_auth::session::priv_session_data *psd = psess.get_session();
			
			/* Make sure username is correct; force it to be
			 * if it isn't */
			std::string username = ph->get_username();
			if (ph->get_username() != psd->get_username()) {
				psd->set_username(username);
			}
			
			/* Update the session from PAM with data stored
			 * in the PAM handle */
			gsp_auth::util::
				update_psd(psd,
					   gsp_auth::util::
					   USERNAME(username),
					   gsp_auth::util::
					   AUTH_TYPE(ph->get_orig_service()),
					   gsp_auth::util::
					   CURR_AUTH_TYPE(ph->get_service()));
			
			/* Update shm with new data */
			gsp_auth::util::sess_update_helper(sinfo, t,
					       	   gsp_auth::session::
						   PRIV_SHMID(priv_shmid),
						   psess);
		
			/* Don't call pam_end() from the destructor; pam_end()
			 * will be called from gsp_auth_pam_end() */
			ph->set_pam_end_on_destroy(false);
			*pamh = ph->get_pamh();
		} else {
			/* Destroy session; not authenticated */
			t.remove(sinfo);
			gsp_auth::util::shm_destroy(SHM_ID(user_shmid)); 
			gsp_auth::util::shm_destroy(SHM_ID(priv_shmid)); 
		}
	} catch (pam_error e) {
		ret = e.get_ret();
	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = PAM_SYSTEM_ERR;
	} catch (...) {
		ret = PAM_SYSTEM_ERR;
	}
	
	try {
		if (ret != PAM_SUCCESS) {
			try {
				ph->close_session();
				ph->setcred(PAM_FLAGS(PAM_DELETE_CRED));
			} catch (...) {}

			*pamh = 0;

			gsp_auth::session::sid_info sinfo(sid);
			std::istringstream blank_is("");
			blank_is >> sinfo;
		}
	} catch (pam_error e) {
		ret = e.get_ret();
	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = PAM_SYSTEM_ERR;
	} catch (...) {
		ret = PAM_SYSTEM_ERR;
	}

	if (ph) {
		delete ph;
		ph = 0;
	}

	return(ret);
}

int gsp_auth_pam_end(pam_handle_t **pamh)
{
	using namespace gsp_auth::pam;
	int ret = PAM_SUCCESS;
	avo::util::syslog_helper sh(gsp_auth::libinfo::get_name(),
				    LOG_PID, LOG_AUTH);

	try {
		pam_helper ph(*pamh);

		ph.close_session();
		ph.setcred(PAM_FLAGS(PAM_DELETE_CRED));

		ph.end();
	} catch (pam_error e) {
		ret = e.get_ret();
 	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		ret = PAM_SYSTEM_ERR;
	} catch (...) {
		ret = PAM_SYSTEM_ERR;
	}

	return(ret);
}

/* Return the "original" service set with pam_start() */
char *gsp_auth_pam_get_orig_service(pam_handle_t *pamh)
{
	char *orig_service = 0;
	
	if (pamh == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": pamh is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(0);
	}
	
	/* Get what we need from PAM */
	try {
		gsp_auth::pam::pam_helper ph(pamh);
		
		if (! ph.get_orig_service().empty()) {
			orig_service = strdup(ph.get_orig_service().c_str());
		}
	} catch (gsp_auth::pam::pam_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}
	
	return(orig_service);
}

/* Return the current service in the PAM handle */
char *gsp_auth_pam_get_service(pam_handle_t *pamh)
{
	char *service = 0;
	
	if (pamh == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": pamh is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(0);
	}
	
	/* Get what we need from PAM */
	try {
		gsp_auth::pam::pam_helper ph(pamh);
		
		if (! ph.get_service().empty()) {
			service = strdup(ph.get_service().c_str());
		}
	} catch (gsp_auth::pam::pam_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (app_error e) {
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, e.get_str());
		return(0);
	} catch (...) {
		return(0);
	}
	
	return(service);
}

/* Populate SID with SID from PAM handle */
int gsp_auth_pam_get_sid(pam_handle_t *pamh, sid_data *sid)
{
	if (pamh == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": pamh is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}

	if (sid == 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": sid is NULL" << std::endl;
		avo::util::syslog_helper::log(gsp_auth::libinfo::get_name(),
					      LOG_ERR, err_msg.str());
		return(ERROR);
	}
	
	/* Get what we need from PAM */
	try {
		gsp_auth::pam::pam_helper ph(pamh);
		
		ph.get_sid(sid);
	} catch (gsp_auth::pam::pam_error e) {
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
/* End Public C API Implementation */
