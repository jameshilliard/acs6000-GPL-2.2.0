#include "gsp-air.h"
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <dlog.h>

#define MAX_AIR_WORKERS 5
pthread_cond_t air_core_worker_cond[ MAX_AIR_WORKERS ];
air_rpc_req* air_core_worker_requests[ MAX_AIR_WORKERS ];
int air_core_worker_count = 0;

air_local_svc* local_svc = NULL;
pthread_mutex_t local_svc_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t air_core_thread = 0;

int air_local_svc_cnt = 0;
int air_local_svc_max = 0;
struct pollfd* air_local_svc_pfd;

inline int air_flush( air_connection* conn )
{
  // WRITE FULL BUFFER OUT!   
  char* buff = conn->write_buffer;
  int len = 1024 - conn->write_buffer_length;
  conn->write_buffer_pos = buff;
  conn->write_buffer_length = 1024;
  
  while ( len > 0 )
  {
    int x = write( conn->fd, buff, len );
    //printf( "REAL WRITE %d\n", x );
    if ( x < 0 )
    {
      if ( errno == EAGAIN ) 
      {
        struct pollfd pfd;
        pfd.fd = conn->fd;
        pfd.events = POLLOUT;
        pfd.revents = 0;

        if ( poll( &pfd, 1, conn->write_timeout ) <= 0 )
        {
          if ( errno != EINTR )
          {
            return ( AIR_ERROR_WRITE_TIMEOUT );
          }
        }
      }
      else
      {
        return ( AIR_ERROR_WRITE_ERROR );
      }        
    }
    else
    {
      buff += x;
      len -= x;
    }
  }
  return ( AIR_ERROR_OK );
}

inline int air_read( air_connection* conn, void* buff, int len )
{
  if ( conn->write_buffer_length < 1024 )
  {
    int ret = air_flush( conn );
    if ( ret )
    {
      return ( ret );
    }
  }
    
  if (!len) {
    *(char *)buff = 0;
     return ( AIR_ERROR_OK );
  }

  for ( ;; )
  {
    int blen = conn->read_buffer_length;
    if ( blen > len )
    {
      memcpy( buff, conn->read_buffer_pos, len );

      conn->read_buffer_length -= len;
      conn->read_buffer_pos += len;

      return ( AIR_ERROR_OK );
    }
    else if( blen == len )
    {
      memcpy( buff, conn->read_buffer_pos, len );

      conn->read_buffer_length = 0;
      conn->read_buffer_pos = conn->read_buffer;

      return ( AIR_ERROR_OK );
    }
    else if ( blen > 0 )
    {
      memcpy( buff, conn->read_buffer_pos, blen );
      buff += blen;
      len -= blen;
      conn->read_buffer_pos = conn->read_buffer;
      conn->read_buffer_length = 0;
    }

    for ( ;; )
    {
      struct pollfd pfd;
      pfd.fd = conn->fd;
      pfd.events = POLLIN;
      pfd.revents = 0;

      if ( poll( &pfd, 1, conn->read_timeout ) <= 0 )
      {
        if ( errno != EINTR )
        {
          return ( AIR_ERROR_READ_TIMEOUT );
        }
      }
          
      int x = read( conn->fd, conn->read_buffer, 1024 );
      //printf( "REAL READ %d\n", x );
      if ( x < 0 )
      {
        if ( errno == EAGAIN ) 
        {
          continue;
        }
        else
        {
          return ( AIR_ERROR_READ_ERROR );
        }
      }
      else if ( x == 0 ) // EOS
      {
        return ( AIR_ERROR_READ_ERROR );
      }
      else
      {
        conn->read_buffer_length = x;
        break;
      }
    }

  }
}

inline int air_write( air_connection* conn, const void* buff, int len )
{
  while ( len > 0 )
  {
    int blen = conn->write_buffer_length;
    if ( blen > len )
    {
      memcpy( conn->write_buffer_pos, buff, len );
      conn->write_buffer_pos += len;
      conn->write_buffer_length -= len;
      return ( AIR_ERROR_OK );
    }
    else if ( blen == len )
    {
      memcpy( conn->write_buffer_pos, buff, len );
      return ( air_flush( conn ) );
    }
    else
    {
      memcpy( conn->write_buffer_pos, buff, blen );
      buff += blen;
      len -= len;
      int ret = air_flush( conn );
      if ( ret != AIR_ERROR_OK )
      {
        return ( ret );
      }
    }
  }
  return ( AIR_ERROR_OK );
}

inline int air_close( air_connection* conn )
{
  dlog_printf( DLOG_DEBUG, NULL, "" );
  
  int ret = air_flush( conn );
  close( conn->fd );
  free( conn );
  return ( ret );
}

air_local_svc* find_local_svc( const char* endpoint )
{
  air_local_svc* cur = local_svc;
  while ( cur )
  {
    if ( ( endpoint == cur->endpoint ) || ( strcmp( endpoint, cur->endpoint ) == 0 ) )
    {
      if ( cur->prev ) // Not first service in list...
      {
        cur->prev->next = cur->next;
        if ( cur->next )
        {
          cur->next->prev = cur->prev;
        }
        cur->next = local_svc;
        cur->prev = NULL;
        local_svc = cur;
      }

      return ( cur );
    }
    cur = cur->next;
  }
  return ( NULL );
}

int air_publish( const char* endpoint, void* data, const void** funcs )
{
  dlog_printf( DLOG_DEBUG, NULL, "Endpoint: %s", endpoint );
  
  // Acquire Local Svc Lock
  pthread_mutex_lock( &local_svc_lock );

  // Verify endpoint is new
  air_local_svc* svc = find_local_svc( endpoint );

  if ( ( svc ) && ( svc->local ) )
  {
    return ( AIR_ERROR_SERVICE_EXISTS );
  }

  // Open UDS Server Socket
  struct sockaddr_un addr;
  mkdir( "/tmp/air", 0777 ); // Make the directory in case it doesn't exist
  sprintf( addr.sun_path, "/tmp/air/%s", endpoint );
  if ( ( unlink( addr.sun_path ) ) && ( errno != ENOENT ) )
  {
    return ( AIR_ERROR_SERVICE_EXISTS );
  }
  addr.sun_family = AF_UNIX;

  int fd = socket( PF_UNIX, SOCK_STREAM, 0 );
  fcntl( fd, F_SETFL, O_NONBLOCK );
  bind( fd, (struct sockaddr*)&addr, strlen( addr.sun_path ) + sizeof( addr.sun_family ) );
  listen( fd, 10 );
  
  // Create Service entry in Service List  
  int newsvc = 0;
  if ( svc == NULL )
  {
    newsvc = 1;
    svc = malloc( sizeof( air_local_svc ) );
  }
  
  if ( svc == NULL )
  {
    return ( AIR_ERROR_ALLOC_FAILED );
  }
  
  svc->endpoint = strdup( endpoint );
  if ( svc->endpoint == NULL )
  {
    free( svc );
    return ( AIR_ERROR_ALLOC_FAILED );
  }
  
  svc->users = 0;
  svc->data = data;
  svc->funcs = funcs;
  svc->fd = fd;
  svc->local = 1;
  pthread_cond_init( &svc->condition, NULL );
  
  if ( newsvc )
  {
    svc->prev = NULL;
    svc->next = local_svc;
    local_svc = svc;
    if( svc->next )
    {
      svc->next->prev = svc;
    }
  }

  // If first service spawn core thread
  if ( air_core_thread == 0 )
  {
    air_local_svc_max = 16;
    air_local_svc_pfd = (struct pollfd*)malloc( sizeof( struct pollfd ) * air_local_svc_max );

    int x;
    for ( x = 0; x < MAX_AIR_WORKERS; x++ )
    {
        pthread_cond_init( &air_core_worker_cond[ x ], NULL );
    }
    
    pthread_cond_t start_thread_cond;
    pthread_cond_init( &start_thread_cond, NULL );
    pthread_create( &air_core_thread, NULL, air_service_core, &start_thread_cond );
    pthread_cond_wait( &start_thread_cond, &local_svc_lock );
    pthread_cond_destroy( &start_thread_cond );
  }

  air_local_svc_cnt++;

  // Release lock
  pthread_mutex_unlock( &local_svc_lock );

  return ( AIR_ERROR_OK );
}

int air_unpublish( const char* endpoint )
{
  dlog_printf( DLOG_DEBUG, NULL, "Endpoint: %s", endpoint );
  
  // Acquire Local Svc Lock
  pthread_mutex_lock( &local_svc_lock );

  // Find Svc Entry
  air_local_svc* svc = find_local_svc( endpoint );
  
  if ( ( svc == NULL ) || ( !svc->local ) )
  {
    pthread_mutex_unlock( &local_svc_lock );
    return ( AIR_ERROR_UNKNOWN_SERVICE );
  } 

  // while ( users > 0 ) {
  while ( svc->users > 0 )
  {
    pthread_cond_wait( &svc->condition, &local_svc_lock );
  }

  // Close Server UDS
  close( svc->fd );

  // Free Service Entry
  local_svc = svc->next;
  if ( svc->next )
  {
    svc->next->prev = NULL;
  }

  air_local_svc_cnt--;

  // Destroy Condition Variable
  pthread_cond_destroy( &svc->condition );
  
  // Release Local Svc Lock
  pthread_mutex_unlock( &local_svc_lock );

  free( (void*)svc->endpoint );
  free( svc );

  return ( AIR_ERROR_OK );
}

void* air_service_worker( air_rpc_req* rpc )
{
  air_connection conn;
  conn.read_timeout = 2000;
  conn.write_buffer_pos = conn.write_buffer;
  conn.write_timeout = 1000;
  conn.read_buffer_pos = conn.read_buffer;
  conn.read_buffer_length = 0;
  conn.write_buffer_length = 1024;
  
  for ( ;; )
  {
    
    // Call RPC Method on given UDS
    conn.fd = rpc->fd;
    // Set Non-blocking!
    fcntl( conn.fd, F_SETFL, O_NONBLOCK );

    int ret = ((int(*)( const void**, const char*, void*, void* ))rpc->svc->funcs[0])( rpc->svc->funcs, rpc->svc->endpoint, rpc->svc->data, &conn );
  
    if ( ret )
    {
      air_write( &conn, &ret, sizeof( int ) );
    }

    air_flush( &conn );
    
    if ( ( ret != AIR_ERROR_READ_ERROR ) && ( ret != AIR_ERROR_READ_TIMEOUT ) )
    {
      // Allow this thread/socket to be reused.. if a request come within 2 sec
      continue;
    }

    pthread_mutex_lock( &local_svc_lock );
    rpc->svc->users--;
    pthread_cond_signal( &rpc->svc->condition );
    pthread_mutex_unlock( &local_svc_lock );
    
    // If request didn't come in.. close the socket.
    close( conn.fd );
    
    pthread_mutex_lock( &local_svc_lock );
    
    // Socket won't be reused.. but thread can still be reused
    if ( air_core_worker_count == MAX_AIR_WORKERS )
    {
      pthread_mutex_unlock( &local_svc_lock );
      free( rpc );
      return ( NULL );
    }

    int idx = air_core_worker_count++;
    air_core_worker_requests[ idx ] = rpc;
    pthread_cond_wait( &air_core_worker_cond[ idx ], &local_svc_lock );
    pthread_mutex_unlock( &local_svc_lock );
  }  
}

void* air_service_core( void* tmp )
{
  dlog_printf( DLOG_DEBUG, NULL, "" );
  
  // Make sure Broken Pipe Errors are ignored.. Unix Domain Sockets can cause
  //   the signal if one of the two sides is out of sync.
  signal( SIGPIPE, SIG_IGN );
  
  // Worker threads are detached so memory can be reclaimed... so create attr
  pthread_attr_t attr;
  pthread_attr_init( &attr );
  pthread_attr_setdetachstate( &attr, 1 );

  pthread_mutex_lock( &local_svc_lock );
  pthread_cond_signal( (pthread_cond_t*)tmp );  
  pthread_mutex_unlock( &local_svc_lock );
  
  for ( ;; )
  {
    // Acquire Lock
    pthread_mutex_lock( &local_svc_lock );

    if ( air_local_svc_cnt > air_local_svc_max )
    {
      free( air_local_svc_pfd );
      air_local_svc_max *= 2;
      air_local_svc_pfd = (struct pollfd*)malloc( sizeof( struct pollfd ) * air_local_svc_max );    
    }

    // Build list of UDS
    int idx = 0;
    air_local_svc* cur = local_svc;
    while ( cur )
    {
      air_local_svc_pfd[ idx ].fd = cur->fd;
      air_local_svc_pfd[ idx ].events = POLLIN;
      air_local_svc_pfd[ idx ].revents = 0;
      idx++;
      cur = cur->next;
    }

    // Release Lock
    pthread_mutex_unlock( &local_svc_lock );

    if ( idx == 0 )
    {
      usleep( 1000 * 100 ); // 100ms
      continue;
    } 
    else
    {
      poll( air_local_svc_pfd, idx, 100 );
    }

    // Acquire Lock
    pthread_mutex_lock( &local_svc_lock );

    // Find pending connect
    cur = local_svc;
    while ( cur )
    {
      // Check
      int nfd = accept( cur->fd, NULL, NULL ); 
      if ( nfd != -1 )
      {
        // Spawn thread to handle request
        air_rpc_req* req;
        int idx = -1;
        
        if ( air_core_worker_count == 0 )
        {
  	  req = (air_rpc_req*)malloc( sizeof( air_rpc_req ) );
        }
        else
        {
          idx = --air_core_worker_count;
          req = air_core_worker_requests[ idx ];
        }

        req->fd = nfd;
        req->svc = cur;
        cur->users++;
        if ( idx == -1 )
        {
          pthread_t thr;        
	  pthread_create( &thr, &attr, (void*(*)(void*))air_service_worker, req );
        }
        else
        {
          pthread_cond_signal( &air_core_worker_cond[ idx ] );
        }
      }
      cur = cur->next;
    }

    // Release Lock
    pthread_mutex_unlock( &local_svc_lock );
  }
  return ( NULL );
}

void* air_alloc( void* ptr, int size )
{
  // Malloc size + prev/next ptr
  struct air_mem_block* block = malloc( sizeof( struct air_mem_block ) + size );

  dlog_printf( DLOG_DEBUG, NULL, "Block: %x", block );

  if ( block == NULL )
  {
    return ( NULL );
  }

  if ( ptr == NULL )
  {
    block->next = NULL;
    block->root = block;   
  }
  else
  {
    struct air_mem_block* ref = (struct air_mem_block*)((char*)ptr - offsetof( struct air_mem_block, data ));
    struct air_mem_block* root = ref->root;
    block->root = root;
    block->next = root->next;
    root->next = block;
  }
  
  return ( &block->data );
}

void air_free( void* ptr )
{
  dlog_printf( DLOG_DEBUG, NULL, "" );
  
  // Get container
  struct air_mem_block* cur = (struct air_mem_block*)((char*)ptr - offsetof( struct air_mem_block, data ));

  // Get root
  cur = cur->root;

  // Loop until pointer NULL
  while ( cur )
  {
    struct air_mem_block* next = cur->next;
    free( cur );
    cur = next;
  }
}

int air_open_service( air_service* svc, const char* endpoint )
{
  signal( SIGPIPE, SIG_IGN );

  // Acquire Local Svc Lock
  pthread_mutex_lock( &local_svc_lock );

  // Check for Svc
  air_local_svc* lsvc = find_local_svc( endpoint );

  // If found
  if ( ( lsvc ) && ( lsvc->local ) )
  {
    // Inc Svc User Count
    lsvc->users++;

    // Release Lock
    pthread_mutex_unlock( &local_svc_lock );

    // Create Service Struct
    svc->local = 1;
    svc->endpoint = lsvc->endpoint;
    svc->data = lsvc->data;
    svc->funcs = lsvc->funcs;

    // return svc
    return ( AIR_ERROR_OK );
  }
  else
  {
    int fd = -1;
    if ( ( lsvc ) && ( lsvc->client_fd_cnt > 0 ) )
    {
      fd = lsvc->client_fd[ --lsvc->client_fd_cnt ];
    }
    
    pthread_mutex_unlock( &local_svc_lock );

    // If success, create Service struct
    svc->local = 0;
    svc->endpoint = endpoint;

    air_connection* conn = (air_connection*)malloc( sizeof( air_connection ) );   
    conn->read_timeout = 10000;
    conn->write_timeout = 1000;
    conn->read_buffer_length = 0;
    conn->read_buffer_pos = conn->read_buffer;
    conn->write_buffer_length = 1024;
    conn->write_buffer_pos = conn->write_buffer;
    conn->fd = fd;
    conn->retry = ( fd == -1 )?1:2;
 
    // Assign Callbacks!
    svc->streamptr = conn;

    // return svc
    return ( AIR_ERROR_OK );
  }
}

int air_connect_service( air_service* svc )
{
  dlog_printf( DLOG_DEBUG, NULL, "" );
  
  air_connection* conn = svc->streamptr;
  
  switch ( conn->retry )
  {
      case 2:
	conn->retry = 1;
	break;
      case 1:
	conn->retry = 0;
	if ( conn->fd >= 0 )
	{
	    close( conn->fd );
	}
	
	// Attempt to connect via Unix Domain Socket
	int fd = socket( PF_UNIX, SOCK_STREAM, 0 );
	// If failed, return NULL
	if ( fd < 0 )
	{
	  return ( AIR_ERROR_UNKNOWN_SERVICE );
	}

	fcntl( fd, F_SETFL, O_NONBLOCK );

	struct sockaddr_un addr;
	sprintf( addr.sun_path, "/tmp/air/%s", svc->endpoint );
	addr.sun_family = AF_UNIX;

	if ( connect( fd, (struct sockaddr*)&addr, strlen( addr.sun_path ) + sizeof( addr.sun_family ) ) )
	{
	  return ( AIR_ERROR_UNKNOWN_SERVICE );
	}
	conn->fd = fd;
	break;
      default:
	return ( AIR_ERROR_UNKNOWN_SERVICE );
  }
  
  return ( AIR_ERROR_OK );
}

int air_close_service( air_service* svc, int reuse )
{
  dlog_printf( DLOG_DEBUG, NULL, "Reuse: %d", reuse );
  
  // If local
  if ( svc->local )
  {
    // Acquire Local Svc Lock
    pthread_mutex_lock( &local_svc_lock );

    // Find Service Record
    air_local_svc* lsvc = find_local_svc( svc->endpoint );

    // Should always be found.. but just in case...
    if ( lsvc )
    {
      // Dec Svc User Count
      lsvc->users--;
    }

    // Release Local Svc Lock
    pthread_mutex_unlock( &local_svc_lock );
  }
  else if ( reuse )
  {
    // Acquire Local Svc Lock
    pthread_mutex_lock( &local_svc_lock );

    // Find Service Record
    air_local_svc* lsvc = find_local_svc( svc->endpoint );

    if ( !lsvc )
    {
      lsvc = (air_local_svc*)malloc( sizeof( air_local_svc ) );
  
      if ( lsvc == NULL )
      {
        air_close( svc->streamptr );
        return ( AIR_ERROR_ALLOC_FAILED );
      }
  
      lsvc->endpoint = strdup( svc->endpoint );
      if ( lsvc->endpoint == NULL )
      {
        free( lsvc );
        air_close( svc->streamptr );
        return ( AIR_ERROR_ALLOC_FAILED );
      }
      
      lsvc->local = 0;
      lsvc->client_fd_cnt = 0;
      lsvc->prev = NULL;
      
      lsvc->next = local_svc;
      local_svc = lsvc;
      if( lsvc->next )
      {
        lsvc->next->prev = lsvc;
      }
    }

    if ( lsvc->client_fd_cnt < 5 )    
    {
        lsvc->client_fd[ lsvc->client_fd_cnt++ ] = svc->streamptr->fd;
    }
    else
    {
        // Close oldest
        close( lsvc->client_fd[ 0 ] );
        
        // Shuffle others
        memcpy( lsvc->client_fd, lsvc->client_fd + 1, 4 * sizeof( int ) );
        
        // Add most recent
        lsvc->client_fd[ 4 ] = svc->streamptr->fd;
    }
    
    free( svc->streamptr );
    
    // Release Local Svc Lock
    pthread_mutex_unlock( &local_svc_lock );
  }
  else
  {
    // Close svc
    air_close( svc->streamptr );    
  }

  return ( AIR_ERROR_OK );
}

