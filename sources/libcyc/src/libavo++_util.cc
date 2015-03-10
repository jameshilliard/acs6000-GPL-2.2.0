/* -*- c++ -*- */
/*****************************************************************
 * File: libavo++_util.cc
 *
 * Copyright (C) 2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: libavo++_util
 *
 ****************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <unistd.h>

#include <cyc++.h>

avo::util::password::password(std::string username) :
	buf(0), buf_sz(sysconf(_SC_GETPW_R_SIZE_MAX))
{
	if (username.empty()) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": username not specified";
		throw app_error(err_msg.str());
	}

	struct passwd *tpwd = 0;

	init();

	int ret = getpwnam_r(username.c_str(), &pw, buf, buf_sz, &tpwd);
	if (ret != 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	if (username != std::string(pw.pw_name)) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": username not found";
		throw app_error(err_msg.str());
	}
}

avo::util::password::password(uid_t uid) :
	buf(0), buf_sz(sysconf(_SC_GETPW_R_SIZE_MAX))
{
	struct passwd *tpwd = 0;

	init();

	int ret = getpwuid_r(uid, &pw, buf, buf_sz, &tpwd);
	if (ret != 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	if (uid != pw.pw_uid) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": user ID not found";
		throw app_error(err_msg.str());
	}
}

avo::util::avogroup::avogroup(std::string groupname) :
	buf(0), buf_sz(sysconf(_SC_GETGR_R_SIZE_MAX))
{
	if (groupname.empty()) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": groupname not specified";
		throw app_error(err_msg.str());
	}

	struct group *tgr = 0;

	init();

	int ret = getgrnam_r(groupname.c_str(), &gr, buf, buf_sz, &tgr);
	if (ret != 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	if (groupname != std::string(gr.gr_name)) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": groupname not found";
		throw app_error(err_msg.str());
	}
}

avo::util::avogroup::avogroup(gid_t gid) :
	buf(0), buf_sz(sysconf(_SC_GETGR_R_SIZE_MAX))
{
	struct group *tgr = 0;

	init();

	int ret = getgrgid_r(gid, &gr, buf, buf_sz, &tgr);
	if (ret != 0) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": " << strerror(errno);
		throw app_error(err_msg.str());
	}

	if (gid != gr.gr_gid) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": group ID not found";
		throw app_error(err_msg.str());
	}
}

std::vector<std::string> avo::util::avogroup::get_groups() const
{
	std::vector<std::string> groups;

        for (; *gr.gr_mem; gr.gr_mem++) {
		groups.push_back(*gr.gr_mem);
        }

	return(groups);
}

avo::util::syslog_helper::syslog_helper(std::string i,
					int option,
					int facility) : ident(i)
{
	openlog(ident.c_str(), option, facility);
}

avo::util::syslog_helper::~syslog_helper()
{
	closelog();
}

void avo::util::syslog_helper::log(int priority, std::string s)
{
	syslog(priority, s.c_str());
}

void avo::util::syslog_helper::log(std::string i, int priority, std::string s)
{
	openlog(i.c_str(), LOG_PID, LOG_LOCAL6);
	syslog(priority, s.c_str());
}
