#include "avct-protocol-core.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

avct_proto_packet* avct_proto_alloc_packet( const unsigned char* sig, int seqNumSize, int maxPayloadSize )
{
  avct_proto_packet* packet = (avct_proto_packet*)malloc( sizeof( avct_proto_packet ) );
  if ( packet == NULL )
  {
    return ( NULL );
  }

  int sigLen = strlen( sig );
  packet->sigSize = sigLen;
  
  int buffSize = ( 256 > 8 + seqNumSize + sigLen )?256: 8 + seqNumSize + sigLen;
  packet->curBufferSize = buffSize;
  packet->buffer = (unsigned char*)malloc( buffSize );
  if ( packet->buffer == NULL )
  {
    free( packet );
    return ( NULL );
  }
  packet->curReadSize = 0;
  packet->seqNumSize = seqNumSize;

  packet->buffer[0] = 0x01;
  memcpy( packet->buffer + 1, sig, sigLen );
  memset( packet->buffer + sigLen + 1, 0x00, seqNumSize + 1 + 4  );
  packet->buffer[ 1 + sigLen + seqNumSize + 1 + 4 ] = 0x0D;
  packet->maxPayloadSize = maxPayloadSize;
  
  return ( packet );
}

void avct_proto_reset_packet( avct_proto_packet* packet )
{
  packet->curReadSize = 0;
  memset( packet->buffer + packet->sigSize + 1, 0x00, packet->seqNumSize + 1 + 4  );
  packet->buffer[ 1 + packet->sigSize + packet->seqNumSize + 1 + 4 ] = 0x0D;
}

void avct_proto_free_packet( avct_proto_packet* packet )
{
  free( packet->buffer );
  free( packet );
}

unsigned char avct_proto_get_command_id( avct_proto_packet* packet )
{
  return ( packet->buffer[ 1 + packet->sigSize + packet->seqNumSize ] );
}

void avct_proto_set_command_id( avct_proto_packet* packet, unsigned char cmd )
{
  packet->buffer[ 1 + packet->sigSize + packet->seqNumSize ] = cmd;
}

int avct_proto_get_sequence( avct_proto_packet* packet )
{
  int x;
  int buff = 0;
  for ( x = 0; x < packet->seqNumSize; x++ )
  {
    buff = (buff << 8) | packet->buffer[ 1 + packet->sigSize + x ];
  }
  return ( buff );
}

void avct_proto_set_sequence( avct_proto_packet* packet, int seq )
{
  int x;
  for ( x = 0; x < packet->seqNumSize; x++ )
  {
    packet->buffer[ packet->sigSize + packet->seqNumSize - x ] = seq & 0xFF;
    seq >>= 8;
  }
}

int avct_proto_get_record_length( avct_proto_packet* packet )
{
  int x;
  int buff = 0;
  for ( x = 0; x < 4; x++ )
  {
    buff = (buff << 8) | packet->buffer[ 1 + packet->sigSize + packet->seqNumSize + 1 + x ];
  }
  return ( buff );
}

void avct_proto_set_record_length( avct_proto_packet* packet, int len )
{
  int x;
  for ( x = 0; x < 4; x++ )
  {
    packet->buffer[ 1 + packet->sigSize + packet->seqNumSize + 1 + 3 - x ] = len & 0xFF;
    len >>= 8;
  }
}

int avct_proto_remove_all_fields( avct_proto_packet* packet )
{
  memset( packet->buffer + packet->sigSize + 1 + packet->seqNumSize + 1, 0x00, 4  );
  packet->buffer[ 1 + packet->sigSize + packet->seqNumSize + 1 + 3 ] = 0x00;
  packet->buffer[ 1 + packet->sigSize + packet->seqNumSize + 1 + 4 ] = 0x0D;
  return ( AVCT_PROTO_OK );
}

int avct_proto_add_field( avct_proto_packet* packet, int fieldId, int size, const void* data )
{
  avct_proto_remove_field( packet, fieldId ); // Remove field in case there was one with the same ID
  int rlen = avct_proto_get_record_length( packet );

  int totalPacketSize = ((rlen==0)?1:rlen) + 1 + 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + ( 3  + size );
  if ( totalPacketSize > packet->curBufferSize )
  {
    int newsize = packet->curBufferSize;
    while ( newsize < totalPacketSize )
    {
      newsize *= 2;
    }
    unsigned char* newbuff = realloc( packet->buffer, newsize );
    if ( newbuff == NULL )
    {
      return ( AVCT_PROTO_ALLOC_FAILURE );
    }
    packet->buffer = newbuff;
  }

  int offset = rlen + 1 + packet->sigSize + packet->seqNumSize + 1 + 4 - ((rlen > 0)?1:0);
  packet->buffer[ offset ] = fieldId & 0xFF;
  packet->buffer[ offset + 1 ] = ( (size >> 8) & 0xFF );
  packet->buffer[ offset + 2 ] = ( size & 0xFF );
  memcpy( packet->buffer + offset + 3, data, size );
  packet->buffer[ offset + 3 + size ] = 0xFF;
  packet->buffer[ offset + 3 + size + 1 ] = 0x0D;
  
  avct_proto_set_record_length( packet, ((rlen==0)?1:rlen) + 3 + size );
  
  return ( AVCT_PROTO_OK );
}

int avct_proto_remove_field( avct_proto_packet* packet, int fieldId )
{
  int size;
  unsigned char* ptr = avct_proto_get_field( packet, fieldId, &size );
  if ( ptr != NULL )
  {
    size += 3;
    ptr -= 3;

    if ( avct_proto_get_record_length( packet ) - size == 1 ) // Last field.. so remove 0xFF
    {
      size += 1;
    }

    int totalPacketSize = avct_proto_get_record_length( packet ) + 1 + 1 + packet->sigSize + packet->seqNumSize + 1 + 4;
    
    memmove( ptr, ptr + size, totalPacketSize - ((ptr + size) - packet->buffer) );
    
    avct_proto_set_record_length( packet, avct_proto_get_record_length( packet ) - size ); // ADJUST SIZE DOWN
  }
  return ( AVCT_PROTO_OK );
}

void* avct_proto_get_field( avct_proto_packet* packet, int fieldId, int* size )
{
  if ( avct_proto_get_record_length( packet ) == 0 )
  {
    return ( NULL );
  }
  
  int offset = 1 + packet->sigSize + packet->seqNumSize + 1 + 4;
  while ( ( packet->buffer[offset] != fieldId ) && ( packet->buffer[offset] != 0xFF ) )
  {
    offset += 3 + (( packet->buffer[ offset + 1 ] << 8) | packet->buffer[ offset + 2 ] );
  }
  
  if ( packet->buffer[ offset ] == 0xFF )
  {
    return ( NULL );
  }
  
  if ( size != NULL )
  {
    *size = (( packet->buffer[ offset + 1 ] << 8) | packet->buffer[ offset + 2 ] );
  }
  
  return ( packet->buffer + offset + 3 );
}

unsigned char* avct_proto_get_raw_packet( avct_proto_packet* packet, int* size )
{
  if ( size != NULL )
  {
    *size = 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + avct_proto_get_record_length( packet ) + 1;
  }
  
  return ( packet->buffer );
}

int avct_proto_read_packet( avct_proto_packet* packet, int size, unsigned char* data, int* consumed )
{
  int bytesRead = 0;
  int ret = AVCT_PROTO_READ_INCOMPLETE;
  
  while ( ret == AVCT_PROTO_READ_INCOMPLETE )
  {
    if ( packet->curReadSize < 1 + packet->sigSize ) // SOH + Signature.. verify match
    {
      int max = (packet->sigSize + 1) - packet->curReadSize;
      int cmp = ( max > size ) ? size : max;
      if ( memcmp( data, packet->buffer + packet->curReadSize, cmp ) != 0 )
      {
        ret = AVCT_PROTO_READ_BAD_PACKET;
      }
      bytesRead += cmp;
      packet->curReadSize += cmp;
      data += cmp;
      size -= cmp;
    }
    else if ( packet->curReadSize < 1 + packet->sigSize + packet->seqNumSize + 1 + 4 ) // Read Seq Number, Command ID, Record Length
    {
      int max = ( 1 + packet->sigSize + packet->seqNumSize + 1 + 4 ) - packet->curReadSize;
      int cmp = ( max > size ) ? size : max;
      memcpy( packet->buffer + packet->curReadSize, data, cmp );
      bytesRead += cmp;
      packet->curReadSize += cmp;
      data += cmp;
      size -= cmp;
      
      // Realloc if neccesary!
      if ( packet->curReadSize == 1 + packet->sigSize + packet->seqNumSize + 1 + 4 )
      {
        int rl = avct_proto_get_record_length( packet );
        if ( rl > packet->maxPayloadSize )
        {
          ret = AVCT_PROTO_READ_TOO_BIG;
        }
        else
        {
          int minSize = 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + rl + 1;
          int buffSize = packet->curBufferSize;
          while ( buffSize < minSize )
          {
            buffSize *= 2;
          }
          if ( buffSize != packet->curBufferSize )
          {
            unsigned char* tmp = realloc( packet->buffer, buffSize );
            if ( tmp == NULL )
            {
              ret = AVCT_PROTO_ALLOC_FAILURE;
            }
            else
            {
              packet->curBufferSize = buffSize;
              packet->buffer = tmp;
            }
          }
        }
      }
      
    }
    else if ( packet->curReadSize < 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + avct_proto_get_record_length( packet ) ) // Record Data
    {      
      int rl = avct_proto_get_record_length( packet );
      if ( rl > packet->maxPayloadSize )
      {
        ret = AVCT_PROTO_READ_TOO_BIG;
      }
      else
      {
        int max = ( 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + rl ) - packet->curReadSize;
        int cmp = ( max > size ) ? size : max;
        memcpy( packet->buffer + packet->curReadSize, data, cmp );
        bytesRead += cmp;
        packet->curReadSize += cmp;
        data += cmp;
        size -= cmp;
      }
    }
    else // Either done.. or need to check packet
    {
      if ( packet->curReadSize < 1 + packet->sigSize + packet->seqNumSize + 1 + 4 + avct_proto_get_record_length( packet ) + 1 )
      {
        // Previous reallocs should have made sure there was room for the 1 byte trailer
        packet->buffer[ packet->curReadSize ] = data[0];
        bytesRead += 1;
        size -= 1;
        packet->curReadSize += 1;
      }
      
      unsigned char* end = packet->buffer + packet->curReadSize - 1; // Points to TRAILER byte
      if ( avct_proto_get_record_length( packet ) > 0 )
      {
        // SHOULD VERIFY RECORD FIELDS!
        unsigned char* cur = packet->buffer + 1 + packet->sigSize + packet->seqNumSize + 1 + 4;      
        while ( ( cur < end ) && ( *cur != 0xFF ) )
        {
          int fsize = (*(cur + 1) << 8) + *(cur + 2);
          cur += fsize + 3;
        }
        
        if ( ( cur != end - 1 ) || ( *cur != 0xFF ) )
        {
          ret = AVCT_PROTO_READ_BAD_PACKET;
        }
        else if ( *end != 0x0D )
        {
          ret = AVCT_PROTO_READ_BAD_PACKET;
        }
        else
        {
          ret = AVCT_PROTO_READ_COMPLETE;
        }
      }      
      else if ( *end != 0x0D )
      {
        ret = AVCT_PROTO_READ_BAD_PACKET;
      }
      else
      {
        ret = AVCT_PROTO_READ_COMPLETE;
      }
    }
    
    if ( size == 0 )
    {
      break;
    }
  }
  
  *consumed = bytesRead;
  
  return ( ret );
}


