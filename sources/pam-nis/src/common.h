/* -*- linux-c -*- */
#ifndef PAM_NIS_COMMON_H
#define PAM_NIS_COMMON_H

/***
  This file is part of pam_dotfile.

  pam_dotfile is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  pam_dotfile is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with pam_dotfile; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA
***/

#include <security/pam_modules.h>
#include <security/_pam_macros.h>

typedef struct context {
	int opt_debug;
	int opt_use_first_pass;
	int opt_try_first_pass;
	int opt_rootok;
	int opt_nullok;
	int opt_fork;  // 0: auto; 1: fork; -1: nofork;
	int opt_no_warn;
	char opt_domain[64];
} context_t;

int user_authentication(context_t *c, const char *username, const char *password);

#endif
