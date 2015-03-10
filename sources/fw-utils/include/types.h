//---------------------------------------------------------------------------
// Copyright 1998 Cybex Computer Products Corporation
//---------------------------------------------------------------------------
// File        : types.h
// Description : define data types, general constants, etc.
//
//---------------------------------------------------------------------------
// Author      : Doug Reed
// Date        : Jun-01-1998
//---------------------------------------------------------------------------
// History
//
// Date        Programmer          Modifications
// ----------- ---------------     ------------------------------------------
// Jun-01-1998 Doug Reed           Created
// Oct 03 2000 D Stafford          Revised to support CPP code
//---------------------------------------------------------------------------

#ifndef __TYPES_H
#define __TYPES_H

#define TRUE  1
#define FALSE 0

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef char CHAR;
typedef void * PVOID;

typedef unsigned long  DWORD;
typedef unsigned long  ULONG;

typedef int BOOL;
typedef int BOOLEAN;
typedef int SOCKET;


#define MAX_COMPUTERNAME_LENGTH 32
#define _MAX_PATH 260


#endif  /* __TYPES_H */

/* end */

