
#ifndef __UTILS_H__
#define __UTILS_H__

/*************** Do NOT alter this file!! *****************/

DWORD Time(void);
char* IPAddrToA(PIPADDR pIPAddr);
char* ENetAddrToA(PENETADDR pENetAddr);

PIPADDR Iface_GetIPAddress(void);
PENETADDR Iface_GetENetAddress(void);
void Iface_Send(PVOID pData, DWORD dwLen);

WORD __ntohs(WORD w);
DWORD __ntohl(DWORD dw);

//FIXME
#ifndef __APPLE__

#if 1
  /* Use 1 if host byte ordering is different from network byte ordering
   * Otherwise use 0
   * "1" is necessary for Intel CPU's, for instance.
   */
	#define htons(x) __ntohs(x)
  #define ntohs(x) __ntohs(x)
	#define htonl(x) __ntohl(x)
  #define ntohl(x) __ntohl(x)
#else
	#define htons(x) (x)
  #define ntohs(x) (x)
	#define htonl(x) (x)
  #define ntohl(x) (x)
#endif

#endif


BOOL KBHit(void);
int Getch(void);


#endif /* __UTILS_H__ */
