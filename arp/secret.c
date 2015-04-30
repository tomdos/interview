

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "typedefs.h"
#include "utils.h"
#include "arp.h"





/************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 * 
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * DO NOT READ THE CONTENTS OF THIS FILE!!!!!
 * 
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************
 ************************************************************************/



#define C6(od,s) Cn(od,s,6)
#define C4(od,s) Cn(od,s,4)
#define Cn(od,s,n) memcpy(chBuf+od,s,n)
#define S6(od, ch) memset(chBuf+od, ch, 6)
#define WP(p) ((WORD*)(p))
#define nWP(p) (*WP(p))
char q[256];
int ql=0;
#ifdef _DEV
#include "dev/dev.inl"
#endif
void Iface_Send(PVOID pData, DWORD dwLen){









/* If, after all, you're reading this. DON'T code this way. 
 * It just looks this way to make it hard for you to read the code :)
 */

void ARP_SimulatReceivedQuery(void);{ BYTE *pch=pData;
if(nWP(pch+16)==8 && dwLen>=42 &&  pch[0]==255 && pch[1]==255 && pch[19]==4 && 
!memcmp(pch+6, (char*)Iface_GetENetAddress(), 6) &&
nWP(pch+12)==0x0608 && pch[4]==255 && pch[5]==255 && nWP(pch+14)==256 && 
pch[18]==6 && nWP(pch+20)==256 && (rand()&15)!=0 && pch[2]==255 && pch[3]==255 ){ ql=42; memcpy(q, pch, 42); q[6]=rand();q[7]=rand();q[8]=rand();q[9]=rand();q[10]=rand();q[11]=rand();
memcpy(q, pch+6, 6); memcpy(q+22, pch+32, 10); memcpy(q+32, pch+22, 10); memcpy(q+22, q+6, 6); nWP(q+20)=512; }} } PIPADDR Iface_GetIPAddress(void) { static BYTE c[4]={192,168,
11,1}; return (PIPADDR) c; } PENETADDR Iface_GetENetAddress(void) { static BYTE c[6]={0,32, 0x1a, 0x22, 0x7f, 0x9b};  return (PENETADDR) c; } 
void ARP_SimulateReceivedQuery(void) { BYTE chBuf[42]; int n; for(n=0;n<42;n++) chBuf[n]=(BYTE)rand(); C6(22,chBuf); S6((n&129),0xff);
S6((n=32), 0); nWP(chBuf+12)=(193<<3); n+=6; C4(n,Iface_GetIPAddress()); n-=6; chBuf[-13+n]=4; chBuf[50-n]=chBuf[(n>>1)+3]+2;
nWP(chBuf+16)=8; nWP(chBuf+14)=nWP(chBuf+(n>>1)+4)=256; ARP_ProcessIncoming(chBuf, 42); } void Secret_SecondProcessing(void) { if(ql) { ARP_ProcessIncoming(q, ql); ql=0; } }


