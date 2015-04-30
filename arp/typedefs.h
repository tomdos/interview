
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

/*************** Do NOT alter this file!! *****************/

#define TRUE  (1)
#define FALSE  (0)

typedef int BOOL;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned long int DWORD;
typedef void *PVOID;

typedef struct tagIPADDR
{
  BYTE v[4];
} IPADDR, *PIPADDR;

typedef struct tagENETADDR
{
  BYTE v[6];
} ENETADDR, *PENETADDR;

typedef struct tagENETHDR
{
  ENETADDR HwDest;
  ENETADDR HwSender;
  WORD wProto;
} ENETHDR, *PENETHDR;



#endif /* __TYPEDEFS_H__ */

