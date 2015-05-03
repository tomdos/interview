
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


/****************************************************************************
 *
 * Function : ARP_TableRecordFind
 *
 * Purpose  : Find a record which contains IP and HW addr.
 *
 * Returns  : Table record for requested IP or NULL
 *
 ***************************************************************************/
PARPRECORD ARP_TableRecordFind(PIPADDR pIPAddr)
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


/****************************************************************************
 *
 * Function : ARP_TableRecordGet
 *
 * Purpose  : Get a suitable record from the table which will be
 *            replaced/updated.
 *
 * Returns  : Record (PARPRECORD) from the table.
 *
 ***************************************************************************/
PARPRECORD ARP_TableRecordGet(PIPADDR pIPAddr)
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

  return &arpTable.table[idx];
}


/****************************************************************************
 *
 * Function : ARP_TableRecordAdd, ARP_TableRecordAddNotAck
 *
 * Purpose  : Add new (or update older) pair of IP and HW addr to the table.
 *            Add or update record but set status to FAIL.
 *
 * Returns  : -
 *
 ***************************************************************************/
void ARP_TableRecordAdd(PIPADDR pIPAddr, PENETADDR pHw)
{
  PARPRECORD pTable;

  pTable = ARP_TableRecordGet(pIPAddr);
  memcpy(&pTable->ip, pIPAddr, sizeof(IPADDR));
  memcpy(&pTable->hw, pHw, sizeof(ENETADDR));
  pTable->timestamp = Time();
  pTable->status = ARP_TABLERECORD_ACTIVE;
}

void ARP_TableRecordAddNotAck(PIPADDR pIPAddr)
{
  PARPRECORD pTable;

  pTable = ARP_TableRecordGet(pIPAddr);
  memcpy(&pTable->ip, pIPAddr, sizeof(IPADDR));
  memset(&pTable->hw, 0xFFFFFFFF, sizeof(ENETADDR));
  pTable->timestamp = Time();
  pTable->status = ARP_TABLERECORD_FAIL;
}

/****************************************************************************
 *
 * Function : ARP_TableAgeing
 *
 * Purpose  : Remove every records older then ARP_LIFE_TIME (30s).
 *
 * Returns  : -
 *
 ***************************************************************************/
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


/****************************************************************************
 *
 * Function : ARP_DBGPacketPrint, ARP_DBGPrintWhoAmI
 *
 * Purpose  : DEBUG - Print packet info. Print host IP and HW addr.
 *
 * Returns  : -
 *
 ***************************************************************************/
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


/****************************************************************************
 *
 * Function : ARP_ProcessIncoming
 *
 * Purpose  : Process incoming packets of ARP proto. Validate packet format,
 *            and add values from packet into the table. In case of request
 *            packet send reply.
 *
 * Returns  : -
 *
 ***************************************************************************/
void ARP_ProcessIncoming(PVOID pData, DWORD dwLen)
{
  PENETHDR pEth;
  PARPHDR pArp;
  PIPADDR pLocalIP;

  pEth = pData;
  pArp = pData + sizeof(ENETHDR);

  //printf("ARP_ProcessIncoming.\r\n");
  DBGPrintWhoAmI();
  DBGPacketPrint(pData, dwLen);

  /* Check basic structure of incommint pkt */
  if (dwLen >= ARP_PACKET_SIZE &&
      ntohs(pEth->wProto) == ETHHDR_TYPE &&
      ntohs(pArp->hrd) == ARPHDR_HTYPE &&
      ntohs(pArp->pro) == ARPHDR_PTYPE &&
      pArp->hln == ARPHDR_HLEN &&
      pArp->pln == ARPHDR_PLEN)
  {
    /* Arp target must be my ip. */
    pLocalIP = Iface_GetIPAddress();
    if (!ComparePIP(pLocalIP, &pArp->tpa))
    {
#if DEBUG
      printf("Incomming packet is not for me.\r\n");
#endif
      return;
    }

    /* ARP request */
    if (ntohs(pArp->op) == ARPHDR_OPER_REQUEST)
    {
      ARP_TableRecordAdd(&pArp->spa, &pArp->sha);

      /* Send ARP reply */
      ARP_PacketReply(&pEth->HwSender, &pArp->spa, &pArp->sha);
      DBGPacketPrint(pArpPacket, ARP_PACKET_SIZE);
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


/****************************************************************************
 *
 * Function : ARP_Init, ARP_Cleanup
 *
 * Purpose  : Initialize and clean up memory.
 *
 * Returns  : -
 *
 ***************************************************************************/
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


/****************************************************************************
 *
 * Function : ARP_SecondProcessing
 *
 * Purpose  : Shall be called every second and process table ageing.
 *
 * Returns  : -
 *
 ***************************************************************************/
void ARP_SecondProcessing(void)
{
  ARP_TableAgeing();
}


/****************************************************************************
 *
 * Function : ARP_PrintAll
 *
 * Purpose  : Print content of the table.
 *
 * Returns  : -
 *
 ***************************************************************************/
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
      continue;

    printf("%1d  %-15s %s  %2lu\r\n",
      pRecord->status,
      IPAddrToA(&pRecord->ip),
      ENetAddrToA(&pRecord->hw),
      (pRecord->status == ARP_TABLERECORD_ACTIVE)
        ? now - pRecord->timestamp : 0);
  }
}


/****************************************************************************
 *
 * Function : ARP_PacketFillValues, ARP_PacketReply, ARP_PacketRequest
 *
 * Purpose  : Fill request/reply packet with all data. Functions don't return
              any value and the result is stored in global variable
              pArpPacket.
 *
 * Returns  : -
 *
 ***************************************************************************/
void ARP_PacketFillValues(PENETADDR pEthLayerDest,
                          PIPADDR pIPDest,
                          PENETADDR pHwDest,
                          WORD op)
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
   pArp->op = htons(op);
   memcpy(&pArp->spa, pIPSrc, sizeof(IPADDR));
   memcpy(&pArp->sha, pHwSrc, sizeof(ENETADDR));
   memcpy(&pArp->tpa, pIPDest, sizeof(IPADDR));
   memcpy(&pArp->tha, pHwDest, sizeof(ENETADDR));
}

void ARP_PacketReply(PENETADDR pEthLayerDest, PIPADDR pIPDest, PENETADDR pHwDest)
{
  ARP_PacketFillValues(pEthLayerDest, pIPDest, pHwDest, ARPHDR_OPER_REQUEST);

  //ARP_DBGPacketPrint(pArpPacket, ARP_PACKET_SIZE);
}

void ARP_PacketRequest(PIPADDR pIPDest)
{
  ENETADDR ethLayerDest;
  ENETADDR hwDest;

  /* Any value in tha */
  memset(&hwDest, 0, sizeof(ENETADDR));
  /* Eth broadcast */
  memset(&ethLayerDest, 0xFFFFFFFF, sizeof(ENETADDR));
  ARP_PacketFillValues(&ethLayerDest, pIPDest, &hwDest, ARPHDR_OPER_REQUEST);

  //ARP_DBGPacketPrint(pArpPacket, ARP_PACKET_SIZE);
}


/****************************************************************************
 *
 * Function : ARP_Query
 *
 * Purpose  : Process query for input IP. Try to search the table at first.
 *            If the table doens't contain requested IP than send ARP
 *            request.
 *
 * Returns  : Return HW addr for requested IP, 0 if the table doesn't
 *            contain IP, -1 in case of error.
 *
 ***************************************************************************/
PENETADDR ARP_Query(PIPADDR pIPAddr)
{
  PARPRECORD pTable;
  PENETADDR ret;

  ret = (PENETADDR) 0;
  pTable = ARP_TableRecordFind(pIPAddr);
  if (pTable)
  {
    if (pTable->status == ARP_TABLERECORD_FAIL)
    {
      ret = (PENETADDR) -1;
    }
    else
    {
      return (PENETADDR) &pTable->hw;
    }
  }

  ARP_TableRecordAddNotAck(pIPAddr);
  ARP_PacketRequest(pIPAddr);
  DBGPacketPrint(pArpPacket, ARP_PACKET_SIZE);
  Iface_Send(pArpPacket, ARP_PACKET_SIZE);

  return ret;
}
