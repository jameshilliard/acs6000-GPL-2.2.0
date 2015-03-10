/* -*- linux-c -*- */
/*****************************************************************
 * File: cyc.h
 *
 * Copyright (C) 2004-2007 Avocent Corporation
 *
 * www.avocent.com
 *
 * Description: libcyc header
 *      
 ****************************************************************/

#ifndef LIBCYC_H
#define LIBCYC_H 1

#ifndef LICENSE
#define LICENSE "Avocent"
#endif

#define LIBCYC_VERSION "3.1"

#include <avo_defs.h>

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#ifdef _REENTRANT
#include <pthread.h>
#endif

#include <sys/types.h>

typedef struct _libcyc_list {
	void *data;
	struct _libcyc_list *next;
	struct _libcyc_list *prev;
	struct _libcyc_list *active;
	struct _libcyc_list *last;
} libcyc_list;

#define CYC_CLEAR 0      /* No flags set */
#define	CYC_INIT  1<<0    /* Is the library initialized? */
#define	CYC_DEBUG  1<<1   /* Debug mode */
#define	CYC_CLIENT  1<<2  /* Initialize as a client */
#define	CYC_SERVER  1<<3  /* Initialize as a server */
#define	CYC_DAEMON  1<<4  /* Initialize as a daemon */
#define	CYC_CGI  1<<5     /* Initialize as a CGI */
#define	CYC_USE_SYSLOG  1<<6 /* Use syslog for output */
#define	CYC_USE_STDERR  1<<7 /* Use stderr for output */
#define	CYC_DEV_PERSIST  1<<8 /* Devs stay open until libcyc_destroy() */
#define	CYC_NOLOCK  1<<9  /* Don't use lockfile */
#define	CYC_CONF_NOUPDATE  1<<10 /* Use current config */

struct shmem_info {
	int id;
	void *mem;
};

struct sem_info {
	int id;
	int nsems;
};

struct dev_info {
	char dev_filepath[MAX_STR_LEN];
	char name[MAX_STR_LEN];
	int fd;
	int semid;
};

struct cyc_info {
	int flags;
	int e_pipe[2];
	char appname[MAX_STR_LEN];
	char appver[MAX_STR_LEN];
	char lockdir[MAX_STR_LEN];
	char lock_filepath[2*MAX_STR_LEN];
	char lock_daemon_filepath[2*MAX_STR_LEN];
	char lock_cgi_filepath[2*MAX_STR_LEN];
	char tmpdir[MAX_STR_LEN];
	char sock_filepath[2*MAX_STR_LEN];
	libcyc_list *sem_ids;
	libcyc_list *shmem_ids;
	libcyc_list *mutexes;
	libcyc_list *mem_ptrs;
	libcyc_list *devs;
	char *conf_file_md5sum;
#ifdef _REENTRANT
	pthread_mutex_t *m;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif
	
/* Main libcyc functions (init/destroy) */
struct cyc_info *libcyc_init(int,const char *,
			     const char *,const char *, const char *);
void libcyc_destroy(struct cyc_info *);
void libcyc_debug(struct cyc_info *, const char *);
int libcyc_add_flag(struct cyc_info *, int);
int libcyc_del_flag(struct cyc_info *, int);
#define libcyc_get_flags(A) A->flags

/* Libcyc malloc replacement functions */
void *libcyc_malloc(struct cyc_info *, size_t);
void libcyc_free(struct cyc_info *, void *);
void libcyc_free_all(struct cyc_info *);
char *libcyc_strdup(struct cyc_info *, const char *);

/* Stack functions */
int libcyc_push(libcyc_list **, void *);
void *libcyc_pop(libcyc_list **);
void *libcyc_top(libcyc_list **);

/* Linked list functions */
libcyc_list *libcyc_ll_append(libcyc_list *, void *);
libcyc_list *libcyc_ll_remove(libcyc_list *, void *);
void libcyc_ll_free(libcyc_list *);
void *libcyc_ll_iterate(libcyc_list *);
void libcyc_ll_reset(libcyc_list *);
int libcyc_ll_get_size(libcyc_list *);

/* Utility functions */
int libcyc_system_pipe(struct cyc_info *, char *, char **,
		       void (*data_fn)
		       (struct cyc_info *, char *, int, void *), void *);
int libcyc_system_str(struct cyc_info *, char *, char **, char *, int *);
int libcyc_system(struct cyc_info *, char *, char **);
int libcyc_system_quiet(struct cyc_info *, char *, char **);
int libcyc_system_nowait(struct cyc_info *, char *, char **);
char *libcyc_create_filepath(struct cyc_info *, const char *,
                             const char *, const char *);
pid_t libcyc_get_daemon_pid(struct cyc_info *);
pid_t libcyc_get_cgi_pid(struct cyc_info *);
pid_t libcyc_get_pid(struct cyc_info *);

char *libcyc_get_sock_filepath(struct cyc_info *);
int libcyc_file_exist(const char *);
pid_t libcyc_update_pid(struct cyc_info *);
int libcyc_iface_check_local_hn(const char *);
int libcyc_iface_check_local_ipv4(const char *);
const char *libcyc_iface_get_ip_ipv4(const char *);
char *libcyc_iface_get_mac(struct cyc_info *, const char *);
char *libcyc_iface_get_mac_delim(struct cyc_info *, const char *, char);
int libcyc_iface_get_mac_raw(struct cyc_info *, const char *,
			     char **, int *);
char *libcyc_get_lockfile(struct cyc_info *, int);
double libcyc_get_uptime(void);
long libcyc_get_uptime_ms(void);
pid_t libcyc_get_pid_from_runfile(const char *);
int libcyc_lock(struct cyc_info *, const char *);
int libcyc_unlock(struct cyc_info *, const char *);
int libcyc_lock_status(struct cyc_info *, const char *);
int libcyc_open_dev(struct cyc_info *, const char *, int);
void libcyc_close_dev(struct cyc_info *, int);
int libcyc_ping(struct cyc_info *, const char *);
char *libcyc_md5sum_hexstr_get(struct cyc_info *, const char *);

/* IPC functions */
int libcyc_sock_init(struct cyc_info *);
void libcyc_sock_destroy(struct cyc_info *,int);
void *libcyc_sock_recv(struct cyc_info *,int);
int libcyc_sock_send(struct cyc_info *,int,void *);
int libcyc_get_sock_from_sockfile(struct cyc_info *, const char *);
int libcyc_ipc_key_get(struct cyc_info *, const char *, int);
int libcyc_ipc_key_get_private(void);
int libcyc_ipc_shmem_create(struct cyc_info *, key_t, size_t);
int libcyc_ipc_shmem_get(struct cyc_info *, key_t, size_t, int);
void *libcyc_ipc_shmem_attach(struct cyc_info *, int, void *, int);
void libcyc_ipc_shmem_detach(struct cyc_info *, int);
int libcyc_ipc_shmem_destroy(struct cyc_info *, int);
int libcyc_ipc_sem_create(struct cyc_info *, key_t, int);
int libcyc_ipc_sem_get(struct cyc_info *, key_t, int, int);
int libcyc_ipc_sem_op(struct cyc_info *, int, int, int);
int libcyc_ipc_sem_op_flag(struct cyc_info *, int, int, int, int);
int libcyc_ipc_sem_timed_op(struct cyc_info *, int, int, int, long int);
int libcyc_ipc_sem_timed_op_flag(struct cyc_info *, int, int,
				 int, long int, int);
int libcyc_ipc_sem_destroy(struct cyc_info *, int);
int libcyc_ipc_sem_set_all(struct cyc_info *, key_t, int);
int libcyc_ipc_sem_set(struct cyc_info *, int, int, int);
int libcyc_ipc_sem_getval(struct cyc_info *, int, int);
int libcyc_ipc_sem_nsems_get(struct cyc_info *, int);

/* Threading functions */
pthread_t libcyc_thread_create(struct cyc_info *,
			       void *(*)(void *),void *);
int libcyc_thread_exit(struct cyc_info *);
int libcyc_thread_cancel(struct cyc_info *, pthread_t);
void *libcyc_thread_join(struct cyc_info *, pthread_t);
int libcyc_thread_detach(struct cyc_info *, pthread_t);
int libcyc_thread_kill(struct cyc_info *, pthread_t, int);
void libcyc_thread_testcancel(void);
pthread_mutex_t *libcyc_mutex_create(struct cyc_info *);
int libcyc_mutex_lock(struct cyc_info *, pthread_mutex_t *);
int libcyc_mutex_unlock(struct cyc_info *, pthread_mutex_t *);
int libcyc_mutex_trylock(struct cyc_info *, pthread_mutex_t *);
void libcyc_mutex_destroy(struct cyc_info *, pthread_mutex_t *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBCYC_H */
