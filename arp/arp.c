
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

void ARP_DBGPacketPrint(PVOID pData, DWORD dwLen)
{
  PENETHDR pEth;
  PARPHDR pArp;

  pEth = pData;
  pArp = pData + sizeof(ENETHDR);

  if (dwLen < sizeof(ENETHDR) + sizeof(ARPHDR)) {
    printf("Pkt is malformed.\r\n");
    return;
  }
  printf("=ETHERNET=\r\n");
  printf("dst: %s\r\n", ENetAddrToA(&pEth->HwDest));
  printf("src: %s\r\n", ENetAddrToA(&pEth->HwSender));
  printf("pro: %04X\r\n", ntohs(pEth->wProto));
  printf("=ARP=\r\n");
  printf("hrd: %d\r\n", ntohs(pArp->hrd));
  printf("pro: %04X\r\n", ntohs(pArp->pro));
  printf("hln: %d\r\n", pArp->hln);
  printf("pln: %d\r\n", pArp->pln);
  printf("op:  %d\r\n", ntohs(pArp->op));
  printf("spa: %s sha: %s\r\n", IPAddrToA(&pArp->spa), ENetAddrToA(&pArp->sha));
  printf("tpa: %s sha: %s\r\n", IPAddrToA(&pArp->tpa), ENetAddrToA(&pArp->tha));
}


void ARP_DBGPrintWhoAmI(void)
{
  printf("Me %s = %s\r\n", IPAddrToA(Iface_GetIPAddress()),
    ENetAddrToA(Iface_GetENetAddress()));
}


void ARP_ProcessIncoming(PVOID pData, DWORD dwLen)
{
  PENETHDR pEth;
  PARPHDR pArp;

  pEth = pData;
  pArp = pData + sizeof(ENETHDR);

  printf("ARP_ProcessIncoming.\r\n");
  ARP_DBGPrintWhoAmI();
  ARP_DBGPacketPrint(pData, dwLen);

  if (dwLen >= ARP_PACKET_SIZE &&
      ntohs(pEth->wProto) == ETHHDR_TYPE &&
      ntohs(pArp->hrd) == ARPHDR_HTYPE &&
      ntohs(pArp->pro) == ARPHDR_PTYPE &&
      pArp->hln == ARPHDR_HLEN &&
      pArp->pln == ARPHDR_PLEN)
  {
    /* ARP request */
    if (ntohs(pArp->op) == ARPHDR_OPER_REQUEST)
    {
      ARP_TableRecordAdd(&pArp->spa, &pArp->sha);
    }
    /* ARP reply */
    else if (ntohs(pArp->op) == ARPHDR_OPER_REPLY)
    {
      ARP_TableRecordAdd(&pArp->spa, &pArp->sha);
    }
    /* Unsupported */
    else
    {
      printf("Unsupported arp operation.\r\n");
    }
  }
  else
  {
    printf("Unsupported packet format.\r\n");
  }


  //FIXME test - protocol and hardware size
  //FIXME test - opcode (request)
  //FIXME send response to a request??
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
  pEth->wProto = htons(ETHHDR_TYPE);

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
