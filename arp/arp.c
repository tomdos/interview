
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

void ARP_PacketReply(PENETADDR, PIPADDR, PENETADDR);
void ARP_PacketRequest(PIPADDR);


ARPTABLE arpTable;
PVOID pArpPacket;

/*
 * Return table record for IP or NULL.
 */
PARPRECORD ARP_TableRecordGet(PIPADDR pIPAddr)
{
  WORD i;

  for (i = 0; i < ARP_TABLE_SIZE; i++)
  {
    if (arpTable.table[i].status == ARP_TABLERECORD_FREE)
      continue;

    if ( *((DWORD *) pIPAddr) == *((DWORD *) &arpTable.table[i].ip))
      return &arpTable.table[i];
  }

  return NULL;
}

void ARP_TableRecordAdd(PIPADDR pIPAddr, PENETADDR pHw)
{
  PARPRECORD pTable;
  WORD idx;
  DWORD oldest;
  DWORD oldestSlot;
  DWORD freeSlot;

  /*
   * Find suitable slot in the arp table. Suitable means:
   * - search for the IP
   * - if IP record doesn't exist search for empty slot
   * - if still nothing use the oldest record
   */
  oldest = 0;
  oldestSlot = ARP_TABLE_SIZE;
  freeSlot = ARP_TABLE_SIZE;

  for (idx = 0; idx < ARP_TABLE_SIZE; idx++)
  {
    pTable = &arpTable.table[idx];

    /* IP */
    if (*((DWORD *) pIPAddr) == *((DWORD *) &pTable->ip))
      break;

    /* Free */
    if (pTable->status == ARP_TABLERECORD_FREE)
      freeSlot = idx;

    /* Oldest */
    if (oldest == 0 || oldest > pTable->timestamp)
    {
      oldest = pTable->timestamp;
      oldestSlot = idx;
    }
  }

  /* Table doens't contain IP - use freeSlot or oldest. */
  if (idx >= ARP_TABLE_SIZE)
  {
    if (freeSlot < ARP_TABLE_SIZE)
      idx = freeSlot;
    else
      idx = oldestSlot;
  }

  /* Set new record (or change older one) */
  pTable = &arpTable.table[idx];
  memcpy(&pTable->ip, pIPAddr, sizeof(IPADDR));
  memcpy(&pTable->hw, pHw, sizeof(ENETADDR));
  pTable->timestamp = Time();
  pTable->status = 1;
}

/* Remove everything older that 30s */
void ARP_TableAgeing()
{
  PARPRECORD pTable;
  DWORD now;
  WORD i;

  now = Time();
  for (i = 0; i < ARP_TABLE_SIZE; i++)
  {
    pTable = &arpTable.table[i];

    if (pTable->status == ARP_TABLERECORD_ACTIVE)
    {
      if (now - pTable->timestamp >= ARP_LIFE_TIME)
        pTable->status = ARP_TABLERECORD_FREE;
    }
  }

}

void ARP_DBGPacketPrint(PVOID pData, DWORD dwLen)
{
  PENETHDR pEth;
  PARPHDR pArp;

  pEth = pData;
  pArp = pData + sizeof(ENETHDR);

  if (dwLen < sizeof(ENETHDR) + sizeof(ARPHDR))
  {
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

      /* Send ARP reply */
      ARP_PacketReply(&pEth->HwSender, &pArp->spa, &pArp->sha);
      ARP_DBGPacketPrint(pArpPacket, ARP_PACKET_SIZE);
      Iface_Send(pArpPacket, ARP_PACKET_SIZE);
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
}


void ARP_Init(void)
{
  memset(&arpTable, 0, sizeof(ARPTABLE));
  pArpPacket = malloc(ARP_PACKET_SIZE);
  if (pArpPacket == NULL)
  {
    ARP_Cleanup();
    exit(EXIT_FAILURE);
  }

  printf("ARP_Init not done.\r\n");
}


void ARP_Cleanup(void)
{
  free(pArpPacket);
  printf("ARP_Cleanup not done.\r\n");
}


void ARP_SecondProcessing(void)
{
  ARP_TableAgeing();
}


void ARP_PrintAll(void)
{
  PARPRECORD pRecord;
  WORD i;
  DWORD now;

  now = Time();

  for (i = 0; i < ARP_TABLE_SIZE; i++)
  {
    pRecord = &arpTable.table[i];

    if (pRecord->status == ARP_TABLERECORD_FREE)
      ;//continue;

    printf("%1d  %-15s %s  %2lu\r\n",
      pRecord->status,
      IPAddrToA(&pRecord->ip),
      ENetAddrToA(&pRecord->hw),
      (pRecord->status == ARP_TABLERECORD_ACTIVE)
        ? now - pRecord->timestamp : 0);
  }
}

void ARP_PacketReply(PENETADDR pEthLayerDest, PIPADDR pIPDest, PENETADDR pHwDest)
{
  PENETHDR pEth;
  PARPHDR pArp;
  PIPADDR pIPSrc;
  PENETADDR pHwSrc;

  memset(pArpPacket, 0, ARP_PACKET_SIZE);

  /* Fill Ethernet  */
  pEth = (PENETHDR) pArpPacket;
  pIPSrc = Iface_GetIPAddress();
  pHwSrc = Iface_GetENetAddress();
  memcpy(&pEth->HwSender, pHwSrc, sizeof(ENETADDR));
  memcpy(&pEth->HwDest, pEthLayerDest, sizeof(ENETADDR));
  pEth->wProto = htons(ETHHDR_TYPE);

  /* Fill ARP */
  pArp = pArpPacket + sizeof(ENETHDR);
  pArp->hrd = htons(ARPHDR_HTYPE);
  pArp->pro = htons(ARPHDR_PTYPE);
  pArp->hln = ARPHDR_HLEN;
  pArp->pln = ARPHDR_PLEN;
  pArp->op = htons(ARPHDR_OPER_REPLY);
  memcpy(&pArp->spa, pIPSrc, sizeof(IPADDR));
  memcpy(&pArp->sha, pHwSrc, sizeof(ENETADDR));
  memcpy(&pArp->tpa, pIPDest, sizeof(IPADDR));
  memcpy(&pArp->tha, pHwDest, sizeof(ENETADDR));
}

void ARP_PacketRequest(PIPADDR pIPDest)
{
  PENETHDR pEth;
  PARPHDR pArp;
  PIPADDR pIPSrc;
  PENETADDR pHwSrc;

  memset(pArpPacket, 0, ARP_PACKET_SIZE);

  /* Fill Ethernet  */
  pEth = (PENETHDR) pArpPacket;
  pIPSrc = Iface_GetIPAddress();
  pHwSrc = Iface_GetENetAddress();
  memcpy(&pEth->HwSender, pHwSrc, sizeof(ENETADDR));
  memset(&pEth->HwDest, 0xFFFFFFFF, sizeof(ENETADDR)); // Broadcast
  pEth->wProto = htons(ETHHDR_TYPE);

  /* Fill ARP */
  pArp = pArpPacket + sizeof(ENETHDR);
  pArp->hrd = htons(ARPHDR_HTYPE);
  pArp->pro = htons(ARPHDR_PTYPE);
  pArp->hln = ARPHDR_HLEN;
  pArp->pln = ARPHDR_PLEN;
  pArp->op = htons(ARPHDR_OPER_REQUEST);
  memcpy(&pArp->spa, pIPSrc, sizeof(IPADDR));
  memcpy(&pArp->sha, pHwSrc, sizeof(ENETADDR));
  memcpy(&pArp->tpa, pIPDest, sizeof(IPADDR));
}


PENETADDR ARP_Query(PIPADDR pIPAddr)
{
  PARPRECORD pTable;

  pTable = ARP_TableRecordGet(pIPAddr);
  if (pTable)
  {
    if (pTable->status == ARP_TABLERECORD_FAIL)
    {
      return (PENETADDR) -1;
    }
    else
    {
      return (PENETADDR) &pTable->hw;
    }
  }

  ARP_PacketRequest(pIPAddr);
  Iface_Send(pArpPacket, ARP_PACKET_SIZE);

  return (PENETADDR) 0;
}
