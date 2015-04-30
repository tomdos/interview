
#ifndef __ARP_H__
#define __ARP_H__

/************************************************************************
 *
 * Here, you may add definitions needed for your implementation.
 *
 ************************************************************************/


void ARP_Init(void);
void ARP_Cleanup(void);
void ARP_SecondProcessing(void);
void ARP_PrintAll(void);

PENETADDR ARP_Query(PIPADDR pIPAddr);
void ARP_ProcessIncoming(PVOID pData, DWORD dwLen);



#endif /* __ARP_H__ */
