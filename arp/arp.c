
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

ARPTABLE arp_table;

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
  printf("ARP_Init not done.\r\n");
}


void ARP_Cleanup(void)
{
  printf("ARP_Cleanup not done.\r\n");
}


void ARP_SecondProcessing(void)
{
  /* Whatever... */
  //printf("Second processing.\r\n");
}


void ARP_PrintAll(void)
{
  printf("ARP_PrintAll not done.\r\n");
}


PENETADDR ARP_Query(PIPADDR pIPAddr)
{
  printf("ARP_Query not done.\r\n");
  return (PENETADDR)0;
}
