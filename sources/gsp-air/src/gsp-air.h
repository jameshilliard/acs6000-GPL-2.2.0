#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <wchar.h>

#ifndef GSPAIR_CORE_H
#define GSPAIR_CORE_H

#define AIR_CORE_ID	((long long)1)

#define AIR_ERROR_OK	0
#define AIR_ERROR_ALLOC_FAILED	-1
#define AIR_ERROR_UNKNOWN_SERVICE	-2
#define AIR_ERROR_UNKNOWN_FUNCTION	-3
#define AIR_ERROR_SERVICE_EXISTS	-4
#define AIR_ERROR_READ_TIMEOUT		-5
#define AIR_ERROR_READ_ERROR		-6
#define AIR_ERROR_WRITE_TIMEOUT		-7
#define AIR_ERROR_WRITE_ERROR		-8

typedef struct _air_local_svc {
  const char* endpoint;
  int users;
  void* data;
  const void** funcs;
  struct _air_local_svc* next;
  struct _air_local_svc* prev;
  pthread_cond_t condition;
  int fd;
  int client_fd[5];
  int client_fd_cnt;
  int local;
} air_local_svc;

struct air_mem_block {
  struct air_mem_block* root;
  struct air_mem_block* next;
  char data[1];
};

typedef struct _air_connection {
  int fd;
  int retry;
  int read_timeout;
  int write_timeout;  
  char write_buffer[ 1024 ];
  char* write_buffer_pos;
  int write_buffer_length;
  char read_buffer[ 1024 ];
  char* read_buffer_pos;
  int read_buffer_length;
} air_connection;

typedef struct _air_rpc_req
{
	int fd;
	air_local_svc* svc;
} air_rpc_req;

typedef struct _air_service
{
	char local;
	const char* endpoint;
	void* data;
	const void** funcs;
	air_connection* streamptr;
} air_service;

int air_publish( const char*, void*, const void** );
int air_unpublish( const char* );
void* air_service_core( void* );
void* air_alloc( void*, int );
void air_free( void* );
int air_open_service( air_service*, const char* );
int air_connect_service( air_service* );
int air_close_service( air_service*, int );

inline int air_read( air_connection* conn, void* buff, int len );
inline int air_write( air_connection* conn, const void* buff, int len );

#endif
