/*****************************************************************
 * File: gsp_auth_pam.cc
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: gsp_auth_pam.cc
 *
 ****************************************************************/

#include <string>
#include <sstream>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include <cyc++.h>
#include <gsp_auth.h>
#include "auth_priv.h"

const std::string gsp_auth::pam::pam_helper::SERVICE_NAME_DEFAULT = "login";
const std::string
gsp_auth::pam::pam_helper::NULL_PAMH_ERR = "Null PAM handle";
const std::string
gsp_auth::pam::pam_helper::NULL_SID_ERR = "Null SID";
const std::string
gsp_auth::pam::pam_helper::PAMH_SID_UNAVAIL_ERR = "SID not set in PAM Handle";

gsp_auth::pam::pam_helper::pam_helper(const PAM_SERVICE_NAME &service_name,
				       const PAM_USERNAME &username,
				       const PAM_TTY_NAME &tty_name,
				       const PAM_CONVERSATION &conv) :
	pamh(0), d_pam_end(true)
{
	struct pam_conv def_conv = {
		misc_conv,
		0,
	};

	int ret = pam_start(service_name.val.empty() ?
			    SERVICE_NAME_DEFAULT.c_str() :
			    service_name.val.c_str(),
			    username.val.empty() ? 0 : username.val.c_str(),
			    conv.val ? conv.val : &def_conv,
			    &pamh);
	check_errcode(ret);
	
	if (! tty_name.val.empty()) {
		ret = pam_set_item(pamh, PAM_TTY, tty_name.val.c_str());
		check_errcode(ret);
	}
}

void gsp_auth::pam::pam_helper::end(int pam_status)
{
	if (pamh) {
		int ret = pam_end(pamh, pam_status);
		check_errcode(ret);
	}
}

std::string gsp_auth::pam::pam_helper::get_username() const
{
	const void *uname_data = 0;
	const char *username = 0;

	int ret = pam_get_item(pamh, PAM_USER, &uname_data);
	check_errcode(ret);

	username = reinterpret_cast<const char *>(uname_data);
	if (username) return(username);

	return("");
}

std::string gsp_auth::pam::pam_helper::get_service() const
{
	const void *srvc_data = 0;
	const char *srvc = 0;

	int ret = pam_get_item(pamh, PAM_SERVICE, &srvc_data);
	check_errcode(ret);

	srvc = reinterpret_cast<const char *>(srvc_data);
	if (srvc) return(srvc);
	
	return("");
}

std::string gsp_auth::pam::pam_helper::get_orig_service() const
{
	const void *orig_srvc_data = 0;
	const gsp_auth_orig_service *orig_srvc = 0;

	int ret = pam_get_item(pamh, PAM_ORIG_SERVICE, &orig_srvc_data);
	check_errcode(ret);

	orig_srvc = reinterpret_cast<const gsp_auth_orig_service *>
		(orig_srvc_data);
	if (orig_srvc) return(orig_srvc->orig_service);
	
	return("");
}

void gsp_auth::pam::pam_helper::get_sid(sid_data *sdata) const
{
	if (sdata == 0) throw app_error(NULL_SID_ERR);

	const void *data = 0;

	int ret = pam_get_item(pamh, PAM_SID, &data);
	check_errcode(ret);

	const sid_data *new_sdata = reinterpret_cast<const sid_data *>(data);

	if (new_sdata) {
		*sdata = *new_sdata; // copy
	} else {
		throw app_error(PAMH_SID_UNAVAIL_ERR);
	}
}

void gsp_auth::pam::pam_helper::set_ruser(const PAM_RUSER_NAME &ruser)
{
	int ret = pam_set_item(pamh, PAM_RUSER,
			       ruser.val.empty() ? 0 : ruser.val.c_str());
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::set_rhost(const PAM_RHOST_NAME &rhost)
{
	int ret = pam_set_item(pamh, PAM_RHOST,
			       rhost.val.empty() ? 0 : rhost.val.c_str());
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::set_service(const PAM_SERVICE_NAME &srvname)
{
	int ret = pam_set_item(pamh, PAM_SERVICE,
			       srvname.val.empty() ?
			       SERVICE_NAME_DEFAULT.c_str() :
			       srvname.val.c_str());
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::
get_item(int item_type, const void **item) const
{
	int ret = pam_get_item(pamh, item_type, item);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::set_item(int item_type, const void *item)
{
	int ret = pam_set_item(pamh, item_type, item);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::authenticate(const PAM_FLAGS &flags)
{
	int ret = pam_authenticate(pamh, flags.val);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::acct_mgmt(const PAM_FLAGS &flags)
{
	int ret = pam_acct_mgmt(pamh, flags.val);

	if (ret == PAM_NEW_AUTHTOK_REQD) {
		chauthtok(PAM_FLAGS(PAM_CHANGE_EXPIRED_AUTHTOK));
	} else {
		check_errcode(ret);
	}
}

void gsp_auth::pam::pam_helper::chauthtok(const PAM_FLAGS &flags)
{
	int ret = pam_chauthtok(pamh, flags.val);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::setcred(const PAM_FLAGS &flags)
{
	int ret = pam_setcred(pamh, flags.val);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::open_session(const PAM_FLAGS &flags)
{
	int ret = pam_open_session(pamh, flags.val);
	check_errcode(ret);
}

void gsp_auth::pam::pam_helper::close_session(const PAM_FLAGS &flags)
{
	int ret = pam_close_session(pamh, flags.val);
	check_errcode(ret);
}
