/* -*- c++ -*- */
/*****************************************************************
 * File: libcyc++_ipc.cc
 *
 * Copyright (C) 2006 Cyclades Corporation
 *
 * www.cyclades.com
 *
 * Description: libcyc++_ipc
 *
 ****************************************************************/

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <unistd.h>

#include <cyc++.h>
#include <cyc.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun
{
        int val;           
        struct semid_ds *buf;      
        unsigned short int *array;
        struct seminfo *__buf;
};
#endif

using namespace std;

const int cyc_ipc::CYC_IPC_CREAT = IPC_CREAT;
const int cyc_ipc::CYC_IPC_EXCL = IPC_EXCL;
const int cyc_ipc::CYC_IPC_NOWAIT = IPC_NOWAIT;

void cyc_ipc::init_ipc(const string &filepath, const int proj_id)
{
	const char *fp = filepath.c_str();

	if ((ipc_key = ftok(fp, proj_id)) == -1) {
		ostringstream err_msg;
		
		err_msg << __func__ << " filepath: " << filepath;
		err_msg << " proj_id: " << proj_id << " : ";
		err_msg << strerror(errno);
		throw app_error(err_msg.str());
	}
}

void cyc_shmem::create(size_t size, int shmflg)
{
	if (size <= 0) throw app_error("Invalid size specified");
	
	shmid = shmget(ipc_key, size,
		       shmflg ? shmflg : SHM_FLAG::DEFAULT().val);
	if (shmid == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmget " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

void cyc_shmem::destroy()
{
	struct shmid_ds ds;

	if (shmctl(shmid, IPC_RMID, &ds) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

void *cyc_shmem::attach()
{
	void *m = NULL;

	try {
		m = attach(0, 0); // Attach to shmem with RW perms
	} catch (app_error e) {
		try {
			/* Now try attaching with read-only perms */
			m = attach(0, SHM_RDONLY);
		} catch (...) {
			throw;
		}
	} catch (...) {
		throw;
	}

	return(m);
}

void *cyc_shmem::attach(const void *shmaddr, int shmflags)
{
	if (shmid == ERROR) throw app_error("Invalid shmid");

        if ((mem = shmat(shmid, shmaddr, shmflags)) == reinterpret_cast<void *>(-1)) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmat " << strerror(errno);
		err_msg << "\nshmat failed: Could not attach to " << shmaddr;
		throw app_error(err_msg.str());
        }

	return(mem);
}

void cyc_shmem::detach(const SHM_ADDR &addr)
{
	if (addr.val) {
		if (shmdt(addr.val) == -1) {
			ostringstream err_msg;
			err_msg << __func__ << " addr: " << addr.val;
			err_msg << " shmdt: " << strerror(errno);
			throw app_error(err_msg.str());
		}
	}
}

size_t cyc_shmem::get_size() const
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(shmdata.shm_segsz);
}

time_t cyc_shmem::get_ctime() const
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(shmdata.shm_ctime);
}

void cyc_shmem::set_uid(uid_t uid)
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	shmdata.shm_perm.uid = uid;

	if (shmctl(shmid, IPC_SET, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

uid_t cyc_shmem::get_uid()
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(shmdata.shm_perm.uid);
}

void cyc_shmem::set_gid(uid_t gid)
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	shmdata.shm_perm.gid = gid;

	if (shmctl(shmid, IPC_SET, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

uid_t cyc_shmem::get_gid()
{
	shmid_ds shmdata;

	if (shmctl(shmid, IPC_STAT, &shmdata) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": shmctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(shmdata.shm_perm.gid);
}

const int cyc_sem::CYC_SEM_UNDO = SEM_UNDO;

void cyc_sem::update_nsems()
{
	union semun arg;
	semid_ds semdata;

	arg.buf = &semdata;
	if (semctl(semid, 0, IPC_STAT, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	nsems = semdata.sem_nsems;
}

void cyc_sem::create(int num_sems, int init_val, int shm_flg)
{
	create(num_sems, shm_flg);
	set_all(init_val);
}

void cyc_sem::create(int num_sems, int shm_flg)
{
	nsems = num_sems;
	
	if ((semid = semget(ipc_key, nsems, shm_flg)) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semget " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

void cyc_sem::setval(int sem_num, int val)
{
	if (semid == ERROR) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": semid not set";
		throw app_error(err_msg.str());
	}

	union semun arg;
	arg.val = val;
	if (semctl(semid, sem_num, SETVAL, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

void cyc_sem::set_all(int val)
{
	if (semid == ERROR) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": semid not set";
		throw app_error(err_msg.str());
	}

	for (int i = 0; i < nsems; i++) {
		setval(i, val);
	}
}

void cyc_sem::destroy()
{
	if (semid == ERROR) {
		std::ostringstream err_msg;
		err_msg << __func__ << ": semid not set";
		throw app_error(err_msg.str());
	}

	union semun arg;

	if (semctl(semid, 0, IPC_RMID, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_notice(err_msg.str());
	}

	semid = ERROR;
}

int cyc_sem::op(int sem_num, int sem_op, int sem_flg)
{
	struct sembuf sops = { sem_num, sem_op, sem_flg };

	if (semid == ERROR) return(ERROR);

        return(semop(semid, &sops, 1));
}

int cyc_sem::op(int sem_num, int sem_op)
{
	return(op(sem_num, sem_op, 0));
}

int cyc_sem::timed_op(int sem_num, int sem_op, unsigned long int msecs)
{
	return(timed_op(sem_num, sem_op, msecs, 0));
}

#ifdef HAS_SEMTIMEDOP
int cyc_sem::timed_op(int sem_num, int sem_op,
		      unsigned long int msecs, int sem_flg)
{
	struct sembuf sops = { sem_num, sem_op, sem_flg };
	struct timespec timeout = { 0, msecs * 1000000  };

	if (semid == ERROR) return(ERROR);

	return(semtimedop(semid, &sops, 1, &timeout));
}
#else
int cyc_sem::timed_op(int sem_num, int sem_op,
		      unsigned long int msecs, int sem_flg)
{
	int ret = 0;

	if (semid == ERROR) return(ERROR);

	if (msecs < 100) msecs = 100;

	while (msecs >= 0) {
		usleep(100000);
		msecs -= 100;
		ret = op(sem_num, sem_op, IPC_NOWAIT | sem_flg);
		if (ret == -1) {
			// Not successful yet, try again
			continue;
		} else {
			// semop succeeded
			return(0);
		}
	}

	// Timeout expired without performing a successful semop
	return(ERROR);
}
#endif

int cyc_sem::getval(int sem_num)
{
	if (semid == ERROR) return(ERROR);

	union semun arg;

	return(semctl(semid, sem_num, GETVAL, arg));
}

int cyc_sem::getpid(const SEM_NUM &snum)
{
	if (semid == ERROR) return(ERROR);

	union semun arg;

	return(semctl(semid, snum.val, GETPID, arg));
}

void cyc_sem::set_uid(uid_t uid)
{
	union semun arg;
	semid_ds semdata;

	arg.buf = &semdata;
	if (semctl(semid, 0, IPC_STAT, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	arg.buf->sem_perm.uid = uid;

	if (semctl(semid, 0, IPC_SET, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

uid_t cyc_sem::get_uid()
{
	union semun arg;
	semid_ds semdata;

	arg.buf = &semdata;
	if (semctl(semid, 0, IPC_STAT, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(arg.buf->sem_perm.uid);
}

void cyc_sem::set_gid(uid_t gid)
{
	union semun arg;
	semid_ds semdata;

	arg.buf = &semdata;
	if (semctl(semid, 0, IPC_STAT, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	arg.buf->sem_perm.gid = gid;

	if (semctl(semid, 0, IPC_SET, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}
}

uid_t cyc_sem::get_gid()
{
	union semun arg;
	semid_ds semdata;

	arg.buf = &semdata;
	if (semctl(semid, 0, IPC_STAT, arg) == -1) {
		ostringstream err_msg;
		err_msg << __func__ << ": semctl " << strerror(errno);
		throw app_error(err_msg.str());
	}

	return(arg.buf->sem_perm.gid);
}
