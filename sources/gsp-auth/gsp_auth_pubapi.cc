/*****************************************************************
 * File: gsp_auth_pubapi.cc
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_auth_pubapi.cc
 *
 ****************************************************************/

#include <sstream>

#include <unistd.h>
#include <sys/types.h>

#include "auth_priv.h"

const std::string gsp_auth::public_api::api_helper::
NOT_AUTH_APIH_SEM_ERR = "Not authorized to create API sems";

gsp_auth::public_api::api_helper::api_helper() : s(0)
{
	if (geteuid() != 0) return; // Don't set up sems for non-root
	
	avo::util::syslog_helper sh(libinfo::get_name(),
				    LOG_PID, LOG_USER);

	try {
		// Set up the semaphores we'll need
		s = new gsp_auth::util::sem(PROJECT_ID(FTOK_SESSION_SEMS));

		enum gsp_auth::util::ipc_stat semstat =
					gsp_auth::util::STAT_IPC_ERROR;
		semstat = s->screate(SEM_NUM_SEMS(2), SEM_FLAG(0644));
		if (semstat == gsp_auth::util::STAT_IPC_CREATE_EXCL) {
			s->set_all(SEM_VAL(1));
		}
	} catch (app_error e) {
		sh.log(LOG_ERR, e.get_str());
		if (s) {
			delete s;
			s = 0;
		}
		throw;
	} catch (...) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": error handling session semaphore";
		err_msg << std::endl;
		sh.log(LOG_ERR, err_msg.str());
		if (s) {
			delete s;
			s = 0;
		}
		throw;
	}
}
