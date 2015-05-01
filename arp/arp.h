
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

/* Pre-defined values for ARP packet of IP4 on Ethenrnet */
#define ETHHDR_TYPE               0x0806
#define ARPHDR_HTYPE              0x01
#define ARPHDR_PTYPE              0x0800
#define ARPHDR_HLEN               0x06
#define ARPHDR_PLEN               0x04
#define ARPHDR_OPER_REQUEST       0x01
#define ARPHDR_OPER_REPLY         0x02

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

/*
 * Arp table - array is used as a ring buffer.
 */
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
