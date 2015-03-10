/* -*- linux-c -*- */
/*****************************************************************
 * File: libcyc_main.c
 *
 * Copyright (C) 2003-2006 Cyclades Corporation
 *
 * www.cyclades.com
 *
 * Description: libcyc_main
 *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <cyc.h>

static int lock(struct cyc_info *);
static int unlock(struct cyc_info *);

static int lock(struct cyc_info *cyc)
{
        int fd;
	char pid_str[MAX_STR_LEN];
        pid_t pid=ERROR;
	char *filepath = NULL;

	if (cyc == NULL) return(ERROR);

	if (cyc->flags & CYC_DAEMON) {
		filepath = cyc->lock_daemon_filepath;
	} else if (cyc->flags & CYC_CGI) {
		filepath = cyc->lock_cgi_filepath;
	} else {
		filepath = cyc->lock_filepath;
	}

	if (libcyc_file_exist(filepath) == TRUE) {
		int msgid = 0;
		pid = libcyc_get_pid_from_runfile(filepath);
		if (kill(pid,0) == -1) {
			unlink(filepath);
			/* The process did not die cleanly; remove 
			 * the message queue.  State: runfile exists,
			 * process dead */
			if ((msgid = msgget(pid, 0)) != -1) {
				msgctl(msgid, IPC_RMID, NULL);
			}
		}

	}
	if ((fd=open(filepath, O_CREAT|O_EXCL|O_WRONLY, 0644)) == -1) {
		perror("libcyc_lock: open");
		return(ERROR);
	}
	
	snprintf(pid_str, MAX_STR_LEN, "%d\n", getpid());
	write(fd, pid_str, strlen(pid_str));
        close(fd);

	return(0);
}

static int unlock(struct cyc_info *cyc)
{
	char *filepath = NULL;

	if (cyc == NULL) return(ERROR);

	if (cyc->flags & CYC_DAEMON) {
		filepath = cyc->lock_daemon_filepath;
	} else if (cyc->flags & CYC_CGI) {
		filepath = cyc->lock_cgi_filepath;
	} else {
		filepath = cyc->lock_filepath;
	}

	if (unlink(filepath) == -1) {
		perror("libcyc_unlock: unlink");
		return(ERROR);
	}
	
	return(0);
}

void libcyc_debug(struct cyc_info *cyc, const char *str)
{
	if ((cyc == NULL) || (! (cyc->flags & CYC_DEBUG))) return;

	if (cyc->flags & CYC_USE_STDERR) {
		fprintf(stderr, "%s\n", str);
	}
	if (cyc->flags & CYC_USE_SYSLOG) {
		syslog(LOG_DEBUG, "%s", str);
	}
	if (! (cyc->flags | (CYC_USE_STDERR|CYC_USE_SYSLOG))) {
		printf("%s\n", str);
	}
}

struct cyc_info *libcyc_init(int flags,
			     const char *appname,
			     const char *appver,
			     const char *lockdir,
			     const char *tmpdir)
{
	struct cyc_info *cyc=
		(struct cyc_info *)calloc(1, sizeof(struct cyc_info));
	char *str;
	char lock_daemon_appname[MAX_STR_LEN];
	char lock_cgi_appname[MAX_STR_LEN];
	char debug_str[MAX_STR_LEN];
	int error=0;

	cyc->flags = flags;

	if (appname) strncpy(cyc->appname, appname, MAX_STR_LEN);
	cyc->appname[MAX_STR_LEN - 1] = '\0';

	if (appver) strncpy(cyc->appver, appver, MAX_STR_LEN-1);
	cyc->appver[MAX_STR_LEN - 1] = '\0';

	if (lockdir) strncpy(cyc->lockdir, lockdir, MAX_STR_LEN-1);
	cyc->lockdir[MAX_STR_LEN - 1] = '\0';

	if (tmpdir) strncpy(cyc->tmpdir, tmpdir, MAX_STR_LEN-1);
	cyc->tmpdir[MAX_STR_LEN - 1] = '\0';

	snprintf(lock_daemon_appname, MAX_STR_LEN, "%s-daemon", cyc->appname);
	lock_daemon_appname[MAX_STR_LEN-1] = '\0';
	snprintf(lock_cgi_appname, MAX_STR_LEN, "%s-cgi", cyc->appname);
	lock_cgi_appname[MAX_STR_LEN-1] = '\0';

	if ((str=libcyc_create_filepath(cyc, cyc->lockdir,
					lock_daemon_appname, LOCK_EXT))) {
		strncpy(cyc->lock_daemon_filepath, str, MAX_STR_LEN - 1);
		cyc->lock_daemon_filepath[MAX_STR_LEN-1] = '\0';
		libcyc_free(cyc,str);
		str=NULL;
	} else {
		error++;
		goto errors;
	}

	if ((str=libcyc_create_filepath(cyc, cyc->lockdir,
					lock_cgi_appname, LOCK_EXT))) {
		strncpy(cyc->lock_cgi_filepath, str, MAX_STR_LEN - 1);
		cyc->lock_cgi_filepath[MAX_STR_LEN-1] = '\0';
		libcyc_free(cyc,str);
		str=NULL;
	} else {
		error++;
		goto errors;
	}

	if ((str=libcyc_create_filepath(cyc, cyc->lockdir,
					cyc->appname, LOCK_EXT))) {
		strncpy(cyc->lock_filepath, str, MAX_STR_LEN - 1);
		cyc->lock_filepath[MAX_STR_LEN-1] = '\0';
		libcyc_free(cyc,str);
		str=NULL;
	} else {
		error++;
		goto errors;
	}

	if ((str=libcyc_create_filepath(cyc, cyc->tmpdir,
					cyc->appname, SOCK_EXT))) {
		strncpy(cyc->sock_filepath, str, MAX_STR_LEN - 1);
		cyc->sock_filepath[MAX_STR_LEN-1] = '\0';
		libcyc_free(cyc,str);
		str=NULL;
	} else {
		error++;
	}

	if (! (cyc->flags & CYC_NOLOCK)) {
		if (lock(cyc)) {
			error++;
			goto errors;
		}
	}

	libcyc_add_flag(cyc, CYC_INIT);

 errors:
	if (error) {
		snprintf(debug_str, MAX_STR_LEN,
			 "%s: Problem initializing libcyc: %s", __func__,
			 appname);
		debug_str[MAX_STR_LEN-1] = '\0';
		libcyc_debug(cyc, debug_str);
		free(cyc);
		cyc=NULL;
		return(NULL);
	}

	if (cyc->flags & CYC_USE_SYSLOG) {
		openlog(cyc->appname, LOG_PID, LOG_LOCAL6);
	}

	snprintf(debug_str, MAX_STR_LEN,
		 "%s libcyc initialized", __func__);
	libcyc_debug(cyc, debug_str);

	return(cyc);
}

void libcyc_destroy(struct cyc_info *cyc)
{
	char debug_str[MAX_STR_LEN];

	if (cyc && (cyc->flags & CYC_INIT)) {
		if (! (cyc->flags & CYC_NOLOCK)) {
			unlock(cyc);
		}
		snprintf(debug_str, MAX_STR_LEN,
			 "%s libcyc uninitialized: %s", __func__,
			 cyc->appname);
		libcyc_debug(cyc, debug_str);

		if (cyc->flags & CYC_USE_SYSLOG) {
			closelog();
		}

		if (cyc->devs) {
			struct dev_info *dev = NULL;
			
			while ((dev = libcyc_ll_iterate(cyc->devs))) {
				close(dev->fd);
				if (libcyc_ipc_sem_getval(cyc, dev->semid, 0) < 1) {
					/* Release the sem */
					libcyc_ipc_sem_op(cyc, dev->semid, 0, 1);
				}
				libcyc_free(cyc, dev);
				dev = NULL;
			}
			libcyc_ll_free(cyc->devs);
			cyc->devs = NULL;
		}

		libcyc_del_flag(cyc, CYC_INIT);
		libcyc_free_all(cyc);

		free(cyc);
		cyc=NULL;
	}
}

/* This function adds a flag to the set and returns the current set */
int libcyc_add_flag(struct cyc_info *cyc, int flag)
{
	if (cyc) {
		cyc->flags |= flag;
		return(cyc->flags);
	}

	return(ERROR);
}

/* This function deletes a flag to the set and returns the current set */
int libcyc_del_flag(struct cyc_info *cyc, int flag)
{
	if (cyc) {
		cyc->flags &= ~flag;
		return(cyc->flags);
	}

	return(ERROR);
}
