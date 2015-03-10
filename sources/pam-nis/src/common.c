/* -*- linux-c -*- */
/***
  This file is part of pam_nis.

  pam_nis is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  pam_nis is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with pam_nis; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA
***/

#include "config.h"

#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>

#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <rpcsvc/yp_prot.h>
#include <rpcsvc/ypclnt.h>

#include "common.h"
#include "log.h"

char *crypt(const char *key, const char *salt);

#ifdef HAVE_LIBXCRYPT
#include <xcrypt.h>
#endif

static
char *_get_yp_password(const char *line)
{
	char *pass, *q;
	const char *p;

	pass = malloc(strlen(line)+1);

	q = pass; p = line;
	/* skip user: */
	while(*p && *p != ':') p++;
	if(*p) p++;
	/* copy password entry: */
	while(*p && *p != ':') *q++ = *p++;
	*q = 0;

	if(!*p){
		free(pass);
		return NULL;
	}
	return pass;
}

int user_authentication(context_t *c, const char *username, const char *password)
{
	char *domain;
	int err;
	char *val;
	int val_len;
	char *yp_pass;
	char *encpass;

	if(!c->opt_domain || !c->opt_domain[0]){
		err = yp_get_default_domain(&domain);
		if(err){
			logmsg(c, LOG_ERR, "yp_get_default_domain: %s", yperr_string(err));
			return PAM_AUTHINFO_UNAVAIL;
		}
		logmsg(c, LOG_INFO, "using default domain '%s'", domain);
	}else
		domain = c->opt_domain;

	err = yp_match(domain, "shadow.byname", username, strlen(username), &val, &val_len);
	if(err) logmsg(c, LOG_ERR, "yp_match (shadow.byname): %s", yperr_string(err));
	if(err == YPERR_MAP){
		err = yp_match(domain, "passwd.byname", username, strlen(username), &val, &val_len);
		if(err){
			logmsg(c, LOG_ERR, "yp_match (passwd.byname): %s", yperr_string(err));
			if(err == YPERR_KEY)
				return PAM_USER_UNKNOWN;
			else
				return PAM_AUTHINFO_UNAVAIL;
		}
	}else if(err){
		if(err == YPERR_KEY)
			return PAM_USER_UNKNOWN;
		else
			return PAM_AUTHINFO_UNAVAIL;
	}

	yp_pass = _get_yp_password(val);
	if(!yp_pass)
		return PAM_CRED_INSUFFICIENT;

	/* these are just the hashes and not secret since
	   they are spread over the network anyway.
	*/
	logmsg(c, LOG_DEBUG, "yp_pass = %s", yp_pass);

	encpass = crypt(password, yp_pass);
	
	logmsg(c, LOG_DEBUG, "encpass = %s", encpass);

	if(strcmp(encpass, yp_pass) == 0)
		return PAM_SUCCESS;

	free(yp_pass);

	return PAM_AUTH_ERR;
}
