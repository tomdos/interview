
#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"
#include "utils.h"
#include "arp.h"


/************************************************************************
 *
 * This is where you write your ARP implementation.
 *
 ************************************************************************/
#include <string.h>

ARPTABLE arpTable;
PVOID arpPacket;

/*
 * Return table record for IP or NULL.
 */
PARPRECORD ARP_TableRecordGet(PIPADDR pIPAddr)
{
  WORD i;

  for (i = 0; i < ARP_TABLE_SIZE; i++)
  {
    if (arpTable.table[i].status != ARP_TABLERECORD_ACTIVE)
      continue;

    if (((DWORD *) pIPAddr) == ((DWORD *) &arpTable.table[i].ip))
      return &arpTable.table[i];
  }

  return NULL;
}

void ARP_TableRecordAdd(PIPADDR pIPAddr, PENETADDR pHw)
{
  PARPRECORD pTable;

  pTable = &arpTable.table[arpTable.table_idx];
  memcpy(&pTable->ip, pIPAddr, sizeof(IPADDR));
  memcpy(&pTable->hw, pHw, sizeof(ENETADDR));
  pTable->timestamp = Time();
  pTable->status = 1;

  arpTable.table_idx = (arpTable.table_idx + 1) % ARP_TABLE_SIZE;
}


void ARP_PrintWhoIam(void)
{
  printf("Me %s = %s\r\n", IPAddrToA(Iface_GetIPAddress()),
    ENetAddrToA(Iface_GetENetAddress()));
}


void ARP_ProcessIncoming(PVOID pData, DWORD dwLen)
{
  printf("ARP_ProcessIncoming not done.\r\n");
  ARP_PrintWhoIam();

  PENETHDR pEth;
  pEth = pData;
  printf("D: %s\r\n", ENetAddrToA(&pEth->HwDest));
  printf("S: %s\r\n", ENetAddrToA(&pEth->HwSender));
  printf("%04X\r\n", ntohs(pEth->wProto));

  printf("\r\n");

  //FIXME test - protocol and hardware size
  //FIXME test - opcode (request)
  //FIXME send response to a request??

  PARPHDR pArp;
  pArp = pData + sizeof(ENETHDR);
  printf("%d\r\n", ntohs(pArp->hrd));
  printf("%04X\r\n", ntohs(pArp->pro));
  printf("%d\r\n", pArp->hln);
  printf("%d\r\n", pArp->pln);
  printf("%d\r\n", ntohs(pArp->op));
  printf("S: %s = %s\r\n", IPAddrToA(&pArp->spa), ENetAddrToA(&pArp->sha));
  printf("T: %s = %s\r\n", IPAddrToA(&pArp->tpa), ENetAddrToA(&pArp->tha));
}


void ARP_Init(void)
{
  memset(&arpTable, 0, sizeof(ARPTABLE));
  arpPacket = (BYTE *) malloc(ARP_PACKET_SIZE);
  if (arpPacket == NULL) {
    ARP_Cleanup();
    exit(EXIT_FAILURE);
  }

  printf("ARP_Init not done.\r\n");
}


void ARP_Cleanup(void)
{
  free(arpPacket);
  printf("ARP_Cleanup not done.\r\n");
}


void ARP_SecondProcessing(void)
{
  /* Whatever... */
  //printf("Second processing.\r\n");
}


void ARP_PrintAll(void)
{
  PARPRECORD pRecord;
  WORD i;

  printf("ARP Table\r\n");
  for (i = 0; i < ARP_TABLE_SIZE; i++) {
    pRecord = &arpTable.table[i];
    printf("%d %s %s %lu\r\n",
      pRecord->status,
      IPAddrToA(&pRecord->ip),
      ENetAddrToA(&pRecord->hw),
      pRecord->timestamp);
  }
}


PENETADDR ARP_Query(PIPADDR pIPAddr)
{
  PENETHDR pEth;
  PARPHDR pArp;
  PIPADDR pHostIP;
  PENETADDR pHostHw;

  memset(arpPacket, 0, sizeof(ARP_PACKET_SIZE));

  /* Fill HW layer */
  pEth = (PENETHDR) arpPacket;
  pHostIP = Iface_GetIPAddress();
  pHostHw = Iface_GetENetAddress();
  memcpy(&pEth->HwSender, pHostHw, sizeof(ENETADDR));
  memset(&pEth->HwDest, 0xFFFFFFFF, sizeof(ENETADDR)); // Broadcast
  pEth->wProto = htons(ARP_PROTOCOL_ARP);

  /* Fill proto layer */
  pArp = arpPacket + sizeof(ENETHDR);
  pArp->hrd = htons(1);
  pArp->pro = htons(0x0800);
  pArp->hln = 6;
  pArp->pln = 4;
  pArp->op = htons(1);
  memcpy(&pArp->spa, pHostIP, sizeof(IPADDR));
  memcpy(&pArp->sha, pHostHw, sizeof(ENETADDR));
  memcpy(&pArp->tpa, pIPAddr, sizeof(IPADDR));
  memset(&pArp->tha, 0xFFFFFFFF, sizeof(ENETADDR));

  //printf("========================DBG\r\n");
  //ARP_ProcessIncoming(arpPacket, ARP_PACKET_SIZE);

  Iface_Send(arpPacket, ARP_PACKET_SIZE);

  printf("ARP_Query not done.\r\n");
  return (PENETADDR)0;
}
