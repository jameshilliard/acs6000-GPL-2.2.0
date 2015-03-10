#ifndef _AIDP_PROTOCOL_CORE_H
#define _AIDP_PROTOCOL_CORE_H

#define AVCT_PROTO_OK              0
#define AVCT_PROTO_READ_COMPLETE   0
#define AVCT_PROTO_READ_INCOMPLETE 1
#define AVCT_PROTO_READ_BAD_PACKET 2
#define AVCT_PROTO_ALLOC_FAILURE   3
#define AVCT_PROTO_READ_TOO_BIG    4

typedef struct _avct_proto_packet
{
  // Used internally by library to track structure
  int sigSize;  
  int seqNumSize;
  int maxPayloadSize;
  int curReadSize;
  int curBufferSize;
  
  // Holds the actual raw packet
  int len;
  unsigned char* buffer;
} avct_proto_packet;

avct_proto_packet* avct_proto_alloc_packet( const unsigned char* sig, int seqNumSize, int maxPayloadSize );
void avct_proto_reset_packet( avct_proto_packet* packet );
void avct_proto_free_packet( avct_proto_packet* packet );

unsigned char avct_proto_get_command_id( avct_proto_packet* packet );
void avct_proto_set_command_id( avct_proto_packet* packet, unsigned char cmd );
int avct_proto_get_sequence( avct_proto_packet* packet );
void avct_proto_set_sequence( avct_proto_packet* packet, int seq );

int avct_proto_remove_all_fields( avct_proto_packet* packet );
int avct_proto_add_field( avct_proto_packet* packet, int fieldId, int size, const void* data );
int avct_proto_remove_field( avct_proto_packet* packet, int fieldId );
void* avct_proto_get_field( avct_proto_packet* packet, int fieldId, int* size );

unsigned char* avct_proto_get_raw_packet( avct_proto_packet* packet, int* size );
int avct_proto_read_packet( avct_proto_packet* packet, int size, unsigned char* data, int* consumed );

#endif

