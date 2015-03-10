/* -*- linux-c -*- */
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

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>

#include "common.h"
#include "log.h"

#define DELAY 5

int main(int argc, char* argv[])
{
	context_t c;
	char *username;
	static char password[128];
	int r;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
    
	memset(&c, 0, sizeof(context_t));
    
	if (isatty(0) || isatty(1) || isatty(2)) {
		int n;
		struct passwd *pw;
		uid_t uid = getuid();

		pw = getpwuid(uid);

		fprintf(stderr, "This program is not intended to be run in this way.\n");
		logmsg(&c, LOG_WARNING, "user (%s; uid=%i) ran pam-nis-helper on the command line", pw ? pw->pw_name : "???", uid);

		n = 5;
		while ((n = sleep(n)) > 0);
        
		return 1;
	}

	if (argc != 5) {
		logmsg(&c, LOG_WARNING, "Invalid invocation");
		return 4;
	}

	strncpy(c.opt_domain, argv[1], sizeof(c.opt_domain));
	username = argv[2];

	if (!strcmp(argv[3], "debug"))
		c.opt_debug = 1;

	if (!strcmp(argv[4], "no_warn"))
		c.opt_no_warn = 1;

	logmsg(&c, LOG_DEBUG, "%s|%s|%s",
	       c.opt_debug ? "debug" : "nodebug", 
	       c.opt_no_warn ? "no_warn" : "warn"
		);
    
	if (geteuid() != 0) {
		logmsg(&c, LOG_WARNING, "Not run as root, executable is probably not SETUID?");
		return 4;
	}

	logmsg(&c, LOG_DEBUG, "Helper started");

	if (!fgets(password, sizeof(password), stdin)) {
		logmsg(&c, LOG_WARNING, "Failure reading from STDIN");
		return 4;
	}

	r = user_authentication(&c, username, password);

	logmsg(&c, LOG_DEBUG, "Helper exiting with return value %u", r);

	return r;
}
