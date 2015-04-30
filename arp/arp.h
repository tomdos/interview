
#ifndef __ARP_H__
#define __ARP_H__

/************************************************************************
 *
 * Here, you may add definitions needed for your implementation.
 *
 ************************************************************************/

#define ARP_CACHE_SIZE    8   /* By definition in 3.1 */
#define ARP_LIFE_TIME     30  /* By definition in 3.1 */

typedef struct tagARPHDR
{
  WORD hrd;
  WORD pro;
  BYTE hln;
  BYTE pln;
  WORD op;
  
  ENETADDR sha;
  IPADDR spa;
  ENETADDR tha;
  IPADDR tpa;
} ARPHDR, *PARPHDR;


/*
 * Line content of arp cache.
 */
typedef struct tagARPRECORD
{
  IPADDR ip;
  DWORD timestamp;
  ENETADDR mac;
  BYTE status;
} ARPRECORD;

typedef struct tagARPTABLE
{
  ARPRECORD table[ARP_CACHE_SIZE];
} ARPTABLE;

void ARP_Init(void);
void ARP_Cleanup(void);
void ARP_SecondProcessing(void);
void ARP_PrintAll(void);

PENETADDR ARP_Query(PIPADDR pIPAddr);
void ARP_ProcessIncoming(PVOID pData, DWORD dwLen);



#endif /* __ARP_H__ */
