/* -*- c++ -*- */
/*****************************************************************
 * File: cyc++.h
 *
 * Copyright (C) 2006-2007 Cyclades Corporation
 *
 * www.cyclades.com
 *
 * Description: libcyc++ header
 *      
 ****************************************************************/

#ifndef LIBCYCPP_H
#define LIBCYCPP_H 1

#include <iostream>
#include <string>
#include <stack>
#include <cstdio>
#include <csignal>
#include <sstream>
#include <vector>
#include <ctime>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pwd.h>
#include <grp.h>
#include <syslog.h>

#ifdef _REENTRANT
#include <pthread.h>
#endif

#include <cyc.h>

namespace avo {
	namespace util {
		class password;
		class avogroup;
		class log_helper;
		class syslog_helper;
		class dlog_helper;
	}
}

// Info about this library
class libcycpp_info {
	static const std::string LIBCYCPP_NAME;
	static const std::string LIBCYCPP_VERSION;
public:
	static const std::string get_name() { return(LIBCYCPP_NAME); }
	static const std::string get_version() { return(LIBCYCPP_VERSION); }
	static const std::string get_license() { return(LICENSE); }
};

// Exception handlers
class app_exception {
protected:
	std::string str;
public:
	app_exception(std::string s) : str(s) {}
	virtual ~app_exception() {}
	virtual void set(std::string s) { str = s; }
	virtual std::string get_str() { return(str); }
	virtual void print() { std::cerr << str << std::endl; }
};

class app_error : public app_exception {
public:
	app_error(std::string s) : app_exception(s) {}
	void print() const { std::cerr << "Error: " << str << std::endl; }
};

class app_notice : public app_exception {
public:
	app_notice(std::string s) : app_exception(s) {}
	void print() const { std::cout << "Notice: " << str << std::endl; }
};

// Signal handling base class
class signals {
protected:
	static const std::string SIG_FN_INVALID;
	static const std::string SIG_INIT_ERR;
	static const std::string SIG_ADD_ERR;
	static const std::string SIG_HANDLE_ERR;
public:
	virtual ~signals() {}
	virtual void add(int, void (*sig_fn)(int)) = 0;
	virtual void handle() = 0;
};

struct signal_data {
	int signum;
	void (*sig_fn)(int);
};

// C signal handling
// The operation is simple: Add signals (by number) along with the
// desired handler function using add(); call "handle" when you want
// the association to be made with the "signal()" function call
class csignals: public signals {
	std::stack<struct signal_data *> sigs;
	csignals(const csignals &);
	csignals &operator=(const csignals &);
public:
	csignals() {}
	virtual void add(int, void (*sig_fn)(int));
	virtual void handle();
};

// POSIX signal handling
// The basic use, defined in the signals base class, is the same as
// the use for csignals except that sigaddset() and sigaction() are
// used instead of signal().  This class also offers the additional
// features that POSIX signalling provides beyond basic C-style signal
// handling.
class psignals : public signals {
	std::stack<struct signal_data *> sigs;
	psignals(const psignals &);
	psignals &operator=(const psignals &);
protected:
	sigset_t sigset;
public:
	psignals();

	// Virtual functions inherited from signals class
	virtual void add(int, void (*sig_fn)(int));
	virtual void handle();

	// POSIX-specific signal handling
	virtual void add(int); // Adds the signal using sigaddset to sigset
	virtual void clear(); // Clears sigset
	virtual void mask(int); // Set signal mask; uses sigprocmask()
	virtual int wait(); // Wait for sigs in set; return sig caught
	virtual void ignore(); // Ignore sigs in the set
	virtual void ignore(int); // Ignore one signal
};

// POSIX signal handling when using threads
class psignals_th : public psignals {
public:
	virtual void mask(int); // Set signal mask; uses pthread_sigmask()
};

// New types to improve ease-of-use of SysV IPC API
struct IPC_KEY {
	explicit IPC_KEY(key_t n) : val(n) {}
	IPC_KEY() : val(0) {}
	static IPC_KEY PRIVATE() { return(IPC_KEY(IPC_PRIVATE)); }
	static IPC_KEY NEW() { return(IPC_KEY(IPC_PRIVATE)); }
	key_t val;
};

struct PROJECT_ID {
	explicit PROJECT_ID(int n) : val(n) {}
	int val;
};

struct FILEPATH {
	explicit FILEPATH(std::string s) : val(s) {}
	std::string val;
};

class cyc_ipc {
protected:
        key_t ipc_key;
	void init_ipc(const std::string &, const int);
public:
	static const int CYC_IPC_CREAT;
	static const int CYC_IPC_EXCL;
	static const int CYC_IPC_NOWAIT;

	cyc_ipc(std::string f, int proj_id) {
		init_ipc(f, proj_id);
	}
	cyc_ipc(const FILEPATH &f, const PROJECT_ID &proj_id) {
		init_ipc(f.val, proj_id.val);
	}

	cyc_ipc(key_t key) : ipc_key(key) {}
	cyc_ipc(const IPC_KEY &key) : ipc_key(key.val) {}
};

struct SHM_ID {
	explicit SHM_ID(int n) : val(n) {
		if (n < 0) {
			std::ostringstream err_msg;
			err_msg << __func__;
			err_msg << ": Invalid shmid";
			throw app_error(err_msg.str());
		}
	}
	int val;
};

struct SHM_SZ {
	explicit SHM_SZ(size_t n) : val(n) {}
	size_t val;
};

struct SHM_FLAG {
	explicit SHM_FLAG(int n) : val(n) {}
	static SHM_FLAG DEFAULT() { return(SHM_FLAG(IPC_CREAT|0660)); }
	int val;
};

struct SHM_ADDR {
	explicit SHM_ADDR(const void *n) : val(n) {}
	const void *val;
};

// Shared memory using Sys V
class cyc_shmem : public cyc_ipc {
	int shmid;
	void *mem;
	cyc_shmem(const cyc_shmem &);
	cyc_shmem &operator=(const cyc_shmem &);
public:
	cyc_shmem(std::string filepath, int proj_id) : cyc_ipc(filepath,
							       proj_id),
						       shmid(ERROR), mem(0) {}
	cyc_shmem(const FILEPATH &f,
		  const PROJECT_ID &proj_id) : cyc_ipc(f, proj_id),
					       shmid(ERROR), mem(0) {}
	cyc_shmem(int id) : cyc_ipc(0), shmid(id), mem(0) {}
	cyc_shmem(const SHM_ID &id) : cyc_ipc(IPC_KEY(0)),
				      shmid(id.val), mem(0) {}
	cyc_shmem(const IPC_KEY &key) : cyc_ipc(key), shmid(ERROR), mem(0) {}

	virtual ~cyc_shmem() {}

	void create(size_t size) { create(size, 0); }
	virtual void create(const SHM_SZ &sz) { create(sz.val); }
	void create(size_t, int);
	virtual void create(const SHM_SZ &sz, const SHM_FLAG &flg) {
		create(sz.val, flg.val);
	}

	virtual void destroy();

	virtual void *attach();
	void *attach(const void *, int);
	virtual void *attach(const SHM_ADDR &addr, const SHM_FLAG &flg) {
		return(attach(addr.val, flg.val));
	}

	virtual void detach(const SHM_ADDR &);
	virtual void detach() {
		if (mem) {
			detach(SHM_ADDR(mem));
			mem = 0;
		}
	}

	virtual void *getmem() { return(mem); }
	virtual int get_id() const { return(shmid); }
	virtual size_t get_size() const;
	virtual time_t get_ctime() const;
	virtual void set_uid(uid_t);
	virtual uid_t get_uid();
	virtual void set_gid(uid_t);
	virtual uid_t get_gid();
};

struct SEM_NUM_SEMS {
	explicit SEM_NUM_SEMS(int n) : val(n) {
		if (n < 0) {
			std::ostringstream err_msg;
			err_msg << __func__;
			err_msg << ": Invalid number of semaphores";
			throw app_error(err_msg.str());
		}
	}
	static SEM_NUM_SEMS SEM_DONT_CARE() { return(SEM_NUM_SEMS(0)); }
	int val;
};

struct SEM_NUM {
	explicit SEM_NUM(int n) : val(n) {}
	int val;
};

struct SEM_OP {
	explicit SEM_OP(int n) : val(n) {}
	static SEM_OP INCR() { return(SEM_OP(1)); }
	static SEM_OP DECR() { return(SEM_OP(-1)); }
	static SEM_OP WAITFOR_ZERO() { return(SEM_OP(0)); }
	static SEM_OP PEND() { return(SEM_OP(-1)); }
	static SEM_OP REL() { return(SEM_OP(1)); }
	int val;
};

struct SEM_VAL {
	explicit SEM_VAL(int n) : val(n) {}
	explicit SEM_VAL(const SEM_OP &n) : val(n.val) {}
	int val;
};

struct SEM_FLAG {
	explicit SEM_FLAG(int n) : val(n) {}
	static SEM_FLAG NOWAIT() { return(SEM_FLAG(IPC_NOWAIT)); }
	static SEM_FLAG UNDO() { return(SEM_FLAG(SEM_UNDO)); }
	static SEM_FLAG DEFAULT() { return(SEM_FLAG(IPC_CREAT|0660)); }
	int val;
};

struct SEM_TIME_MSEC {
	explicit SEM_TIME_MSEC(unsigned long int n) : val(n) {}
	unsigned long int val;
};

struct SEM_ID {
	explicit SEM_ID(int n) : val(n) {}
	int val;
};

// Semaphores using Sys V
class cyc_sem : cyc_ipc {
	int semid;
	int nsems;
	void update_nsems();
	void create(int, int, int);
	void create(int, int);
public:
	static const int CYC_SEM_UNDO;

	cyc_sem(std::string f, int proj_id) : cyc_ipc(f, proj_id),
					      semid(ERROR), nsems(0) {}
	cyc_sem(const FILEPATH &f, const PROJECT_ID &proj_id)
		: cyc_ipc(f, proj_id), semid(ERROR), nsems(0) {}
	cyc_sem(key_t key) : cyc_ipc(key), semid(ERROR), nsems(0) {}
	cyc_sem(const IPC_KEY &key) : cyc_ipc(key), semid(ERROR), nsems(0) {}
	cyc_sem(const SEM_ID &id) : cyc_ipc(IPC_KEY(0)), semid(id.val),
				    nsems(0) {
		update_nsems(); // Existing sem, update nsems
	}
	virtual ~cyc_sem() {}
	void create(const SEM_NUM_SEMS &num_sems,
		    const SEM_VAL &init_val,
		    const SEM_FLAG &flg) {
		create(num_sems.val, init_val.val, flg.val);
	}
	virtual void create(const SEM_NUM_SEMS &num_sems,
			    const SEM_VAL &init_val) {
		create(num_sems, init_val, SEM_FLAG::DEFAULT());
	}
	virtual void create(const SEM_NUM_SEMS &num_sems) {
		create(num_sems.val, SEM_FLAG::DEFAULT().val);
	}
	virtual void create(const SEM_NUM_SEMS &num_sems,
			    const SEM_FLAG &flg) {
		create(num_sems.val, flg.val);
	}
	virtual void create() {
		create(SEM_NUM_SEMS::SEM_DONT_CARE(), SEM_FLAG::DEFAULT());
	}
	virtual void attach() { create(); }
	void destroy();
	int op(int, int, int);
	virtual int op(const SEM_NUM &snum, const SEM_OP &sop,
		       const SEM_FLAG &sflag) {
		return(op(snum.val, sop.val, sflag.val));
	}
	int op(int, int);
	virtual int op(const SEM_NUM &snum, const SEM_OP &sop) {
		return(op(snum.val, sop.val, 0));
	}
	int timed_op(int, int, unsigned long int, int);
	virtual int timed_op(const SEM_NUM &snum, const SEM_OP &sop,
		     const SEM_TIME_MSEC &msecs, const SEM_FLAG &sflag) {
		return(timed_op(snum.val, sop.val, msecs.val, sflag.val));
	}
	int timed_op(int, int, unsigned long int);
	virtual int timed_op(const SEM_NUM &snum, const SEM_OP &sop,
		     const SEM_TIME_MSEC &msecs) {
		return(timed_op(snum.val, sop.val, msecs.val, 0));
	}
	int getval(int);
	virtual int getval(const SEM_NUM &snum) {
		return(getval(snum.val));
	}
	void setval(int, int);
	virtual void setval(const SEM_NUM &snum, const SEM_VAL &sval) {
		setval(snum.val, sval.val);
	}
	void set_all(int);
	virtual void set_all(const SEM_VAL &sval) {
		set_all(sval.val);
	}
	int get_id() { return(semid); }
	int get_nsems() { return(nsems); }
	int getpid(const SEM_NUM &);
	virtual void set_uid(uid_t);
	virtual uid_t get_uid();
	virtual void set_gid(uid_t);
	virtual uid_t get_gid();
};

// Mutex using POSIX
class cyc_mutex {
	pthread_mutex_t *mutex;
	cyc_mutex(const cyc_mutex &);
	cyc_mutex &operator=(const cyc_mutex &);
 public:
	cyc_mutex();
	~cyc_mutex();
	void lock();
	void unlock();
	void trylock();
};

// Threads using POSIX
class cyc_thread {
	pthread_t thread;
	pthread_attr_t attr;
 public:
	cyc_thread();
	void create(void *(*)(void *));
	void create(void *(*)(void *), void *);
	void exit(int);
	void kill(int);
	int cancel();
	void *join();
	void testcancel();
	pthread_t get_id() { return(pthread_self()); }
	bool equal(pthread_t);
};

// New types for libcyc_app_info class
struct APP_NAME {
	explicit APP_NAME(std::string s) : val(s) {}
	std::string val;
};

struct SYSLOG_NAME {
	explicit SYSLOG_NAME(std::string s) : val(s) {}
	std::string val;
};

struct APP_VERSION {
	explicit APP_VERSION(std::string s) : val(s) {}
	std::string val;
};

struct COPYRIGHT {
	explicit COPYRIGHT(std::string s) : val(s) {}
	std::string val;
};

class libcyc_app_info {
	std::string _APP_NAME;
	std::string _SYSLOG_NAME;
	std::string _APP_VERSION;
	std::string _COPYRIGHT;
public:
	libcyc_app_info(std::string an, std::string sn,
			std::string av, std::string cr) : 
		_APP_NAME(an), _SYSLOG_NAME(sn),
		_APP_VERSION(av),_COPYRIGHT(cr)	{}
	libcyc_app_info(const APP_NAME &an, const SYSLOG_NAME &sn,
			const APP_VERSION &av, const COPYRIGHT &cr) :
		_APP_NAME(an.val), _SYSLOG_NAME(sn.val),
		_APP_VERSION(av.val), _COPYRIGHT(cr.val) {}
	libcyc_app_info() {}
	void set_app_name(std::string an) { _APP_NAME = an; }
	std::string get_app_name() { return(_APP_NAME); }
	void set_syslog_name(std::string sn) { _SYSLOG_NAME = sn; }
	std::string get_syslog_name() { return(_SYSLOG_NAME); }
	void set_app_version(std::string av) { _APP_VERSION = av; }
	std::string get_app_version() { return(_APP_VERSION); }
	void set_copyright(std::string cr) { _COPYRIGHT = cr; }
	std::string get_copyright() { return(_COPYRIGHT); }
};

// Application classes
class app {
public:
	virtual ~app() {}
	virtual void print_title() = 0;
	virtual void print_help() = 0;
};

// Application that will use libcyc
class libcyc_app : public app {
	libcyc_app_info *app_info;
	static int secs_toquit;
	struct cyc_info *cyc;

	inline void init_libcyc(int, std::string, std::string,
				std::string, std::string);
	libcyc_app(const libcyc_app &);
	libcyc_app &operator=(const libcyc_app &);
public:
	static const std::string LIBCYC_INIT_ERR;

	/* Args: Flags, syslog_name, app_version, lock filepath, tempdir */
	libcyc_app(int, std::string, std::string,
		   std::string, std::string); // Deprecated
	libcyc_app(int, libcyc_app_info *, std::string, std::string);
	~libcyc_app();

	cyc_info *get_cyc() { return(cyc); }
	libcyc_app_info *get_app_info() { return(app_info); }

	virtual void print_title();
};

// Command-line utility using libcyc
class libcyc_util : public libcyc_app {
public:
	csignals sighandler;

	libcyc_util(int, std::string, std::string, std::string,
		    std::string);
};

// Base class for utility operations
class libcyc_util_ops {
	libcyc_util_ops(const libcyc_util_ops &);
	libcyc_util_ops &operator=(const libcyc_util_ops &);
protected:
	struct cyc_info *cyc;
public:
	virtual ~libcyc_util_ops() {}
	libcyc_util_ops(struct cyc_info *c) {
		if (c == NULL)
			throw app_error("libcyc unavailable; cannot proceed");
		cyc = c;
	};
};

// Daemon using libcyc
class libcyc_daemon : public libcyc_app {
	int exit_status;
	inline void init_daemon(int);
public:
	psignals_th sighandler;
	cyc_thread sig_th;
	cyc_sem *sig_sem;

	libcyc_daemon(int, std::string, std::string, std::string,
		      std::string, int); // Deprecated
	libcyc_daemon(int, libcyc_app_info *, std::string,
		      std::string, int);
	~libcyc_daemon();
	void quit(int);
	int get_exit_status() { return(exit_status); }
};

// Class to create help screens in command-line programs
class cyc_help {
	static const unsigned int TAB_COLS;
	static const unsigned int COL1_SIZE;
	static const unsigned int COL2_SIZE;
	static const unsigned int COL3_SIZE;

	std::ostringstream help_message;

	void indent();
	void indent(int);
	void break_line(std::string &);
public:
	/* Option, argument, description */
	void add_opt(std::string, std::string, std::string);
	/* Option, description */
	void add_opt(std::string, std::string);
	/* New line only */
	void add_opt();
	void print();
};

class avo::util::password {
	passwd pw;
	char *buf;
	long buf_sz;
	void init() {
		buf = new char[buf_sz];
	}

	password(const password &);
	password &operator=(const password &);
public:
	password(std::string);
	password(uid_t);
	~password() {
		delete[] buf;
		buf = 0;
	}

	std::string get_name() const { return(pw.pw_name); }
	std::string get_passwd() const { return(pw.pw_passwd); }
	uid_t get_uid() const { return(pw.pw_uid); }
	gid_t get_gid() const { return(pw.pw_gid); }
	std::string get_realname() const { return(pw.pw_gecos); }
	std::string get_homedir() const { return(pw.pw_dir); }
	std::string get_shell() const { return(pw.pw_shell); }
};

class avo::util::avogroup {
	mutable group gr;
	char *buf;
	long buf_sz;
	void init() {
		buf = new char[buf_sz];
	}

	avogroup(const avogroup &);
	avogroup &operator=(const avogroup &);
public:
	avogroup(std::string);
	avogroup(uid_t);
	~avogroup() {
		delete[] buf;
		buf = 0;
	}

	std::string get_name() const { return(gr.gr_name); }
	std::string get_passwd() const { return(gr.gr_passwd); }
	gid_t get_gid() const { return(gr.gr_gid); }
	std::vector<std::string> get_groups() const;
};

class avo::util::log_helper {
public:
	virtual ~log_helper() {}

	virtual void log(int, std::string) = 0;
};

class avo::util::syslog_helper : public log_helper {
	std::string ident;
public:
	syslog_helper() {}
	syslog_helper(std::string, int, int);
	~syslog_helper();
	
	void log(int, std::string);
	static void log(std::string, int, std::string);
};

class avo::util::dlog_helper : public log_helper {
public:
};
#endif /* LIBCYCPP_H */
