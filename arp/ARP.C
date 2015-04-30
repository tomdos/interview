
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


void ARP_ProcessIncoming(PVOID pData, DWORD dwLen)
{
  printf("ARP_ProcessIncoming not done.\r\n");
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
