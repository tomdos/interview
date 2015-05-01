
#ifndef __ARP_H__
#define __ARP_H__

/************************************************************************
 *
 * Here, you may add definitions needed for your implementation.
 *
 ************************************************************************/

#define ARP_TABLE_SIZE            8   /* By definition in 3.1 */
#define ARP_LIFE_TIME             30  /* By definition in 3.1 */

#define ARP_TABLERECORD_ACTIVE    0x01
#define ARP_TABLERECORD_INACTIVE  0x02

#define ARP_PACKET_SIZE           (sizeof(ENETHDR) + sizeof(ARPHDR))
#define ARP_PROTOCOL_ARP          0x0806
/*
 * Representation of ARP packet.
 */
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
 * Line content of arp table.
 */
typedef struct tagARPRECORD
{
  IPADDR ip;
  DWORD timestamp;
  ENETADDR hw;
  BYTE status;
} ARPRECORD, *PARPRECORD;

typedef struct tagARPTABLE
{
  ARPRECORD table[ARP_TABLE_SIZE];
  WORD table_idx;
} ARPTABLE, *PARPTABLE;

void ARP_Init(void);
void ARP_Cleanup(void);
void ARP_SecondProcessing(void);
void ARP_PrintAll(void);

PENETADDR ARP_Query(PIPADDR pIPAddr);
void ARP_ProcessIncoming(PVOID pData, DWORD dwLen);



#endif /* __ARP_H__ */
