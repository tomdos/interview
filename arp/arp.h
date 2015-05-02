
#ifndef __ARP_H__
#define __ARP_H__

/************************************************************************
 *
 * Here, you may add definitions needed for your implementation.
 *
 ************************************************************************/

#define ARP_TABLE_SIZE            8   /* By definition in 3.1 */
#define ARP_LIFE_TIME             30  /* By definition in 3.1 */

/* Status of record in the table (ARP cache table). */
#define ARP_TABLERECORD_FREE      0x00
#define ARP_TABLERECORD_ACTIVE    0x01
#define ARP_TABLERECORD_FAIL      0x02

/* Size of ARP packet */
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
 * Representation of ARP header - assuming ARP over eth and ip.
 */
typedef struct tagARPHDR
{
  WORD hrd;     /* HW address space */
  WORD pro;     /* Protocol address space */
  BYTE hln;     /* Hw addr len */
  BYTE pln;     /* Proto addr len */
  WORD op;      /* Opcode */

  ENETADDR sha; /* Sender hw addr */
  IPADDR spa;   /* Sender proto addr */
  ENETADDR tha; /* Target hw addr */
  IPADDR tpa;   /* Target proto addr */
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
 * Arp table - Table/Cache which is used to store arp records containing.
 */
typedef struct tagARPTABLE
{
  ARPRECORD table[ARP_TABLE_SIZE];
} ARPTABLE, *PARPTABLE;


void ARP_Init(void);
void ARP_Cleanup(void);
void ARP_SecondProcessing(void);
void ARP_PrintAll(void);

PENETADDR ARP_Query(PIPADDR pIPAddr);
void ARP_ProcessIncoming(PVOID pData, DWORD dwLen);



#endif /* __ARP_H__ */
