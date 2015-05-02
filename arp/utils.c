#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "typedefs.h"
#include "utils.h"
#include "arp.h"


/*************** BELOW MAY NEED TO BE CHANGED FOR YOUR PLATFORM!!!! */

#ifdef _WIN32
# include <conio.h>
#elif __linux || __APPLE__
# define   _kbhit()  1
# define   _getch()  getchar()
# define sprintf_s(str, size, format, ...)  snprintf(str, size, format, __VA_ARGS__)
#endif

BOOL KBHit(void)
{
  return (BOOL)_kbhit();
}

int Getch(void)
{
  return _getch();
}

DWORD Time(void)
{
  return (DWORD)time(NULL);
}


/*************** ABOVE MAY NEED TO BE CHANGED FOR YOUR PLATFORM!!!! */



/* Forward declarations of functions exported from secret.c */
void ARP_SimulateReceivedQuery(void);
void Secret_SecondProcessing(void);



/****************************************************************************
 *
 * Function : __ntohs, __ntohl
 *
 * Purpose  : Change byte order of a short and long int, respectively
 *
 * Returns  : The byte (and word) swapped result of the original value
 *
 ***************************************************************************/

WORD __ntohs(WORD w)
{
  return (w<<8) | (w>>8);
}

DWORD __ntohl(DWORD dw)
{
  return (dw<<24) | (dw>>24) | ( (dw&0x00ff0000) >> 8 ) | ( (dw&0x0000ff00) << 8);
}



/****************************************************************************
 *
 * Function : ENetAddrToA
 *
 * Purpose  : Translate an ENETADDR value to an ASCIIZ string
 *            The result is stored in an internal, static buffer
 *            that is overwritten each time this function is called.
 *            It is not suitable for multithreading, and cannot
 *            be called multiple times in the same parsing context.
 *
 * Returns  : A char pointer to a static buffer containing the result string
 *
 ***************************************************************************/

char* ENetAddrToA(PENETADDR p)
{
  static char szBuf[20];
  sprintf_s(szBuf, sizeof(szBuf), "%02x%02x:%02x%02x:%02x%02x",
    ((BYTE*)p)[0],
    ((BYTE*)p)[1],
    ((BYTE*)p)[2],
    ((BYTE*)p)[3],
    ((BYTE*)p)[4],
    ((BYTE*)p)[5]
  );
  return szBuf;
}


/****************************************************************************
 *
 * Function : IPAddrToA
 *
 * Purpose  : Translate an IPADDR value to an ASCIIZ string
 *            The result is stored in an internal, static buffer
 *            that is overwritten each time this function is called.
 *            It is not suitable for multithreading, and cannot
 *            be called multiple times in the same parsing context.
 *
 * Returns  : A char pointer to a static buffer containing the result string
 *
 ***************************************************************************/

char* IPAddrToA(PIPADDR p)
{
  static char szBuf[20];
  sprintf_s(szBuf, sizeof(szBuf), "%i.%i.%i.%i",
    ((BYTE*)p)[0],
    ((BYTE*)p)[1],
    ((BYTE*)p)[2],
    ((BYTE*)p)[3]
  );
  return szBuf;
}






/****************************************************************************
 *
 * Function : UserLoop
 *
 * Purpose  : Prompt the user for input, take appropriate action
 *            Call the various xxxx_SecondProcessing() functions
 *            once per second.
 *
 * Returns  : -
 *
 ***************************************************************************/

void UserLoop(void)
{
  static int c;
  static DWORD dwNow=0;

  static IPADDR aIPAddr[10]=
  {
    {192,168,22,5},
    {192,168,22,15},
    {192,168,22,17},
    {192,168,22,19},
    {192,168,22,33},
    {192,168,22,35},
    {192,168,22,47},
    {192,168,22,99},
    {192,168,22,195},
    {192,168,22,222}
  };

  srand((unsigned int)Time());

  printf("ARP Algorithm Tester\r\n");
  printf("Press keys 0--9 to have this computer query the network\r\n"
         "   for one of ten IP addresses\r\n");
  printf("Press R to simulate receiving an ARP query.\r\n");
  printf("Press L to list ARP cache contents\r\n");
  printf("Press [esc] to exit\r\n");
  printf("\r\n>");

  while(TRUE)
  {
    if(Time()!=dwNow)
    {
      int nRand=rand()&1;
      dwNow=Time();
      if(nRand)
        Secret_SecondProcessing();
      ARP_SecondProcessing();
      if(!nRand)
        Secret_SecondProcessing();
    }


    if(!KBHit())
      continue;
    c=Getch();
    if(c==27)
    {
      printf("\r   \r\n");
      return;
    }
    else if(c>='0' && c<='9')
    {
      int n = c-'0';
      PENETADDR pENetAddr;
      printf("Query for address %s returns : ", IPAddrToA(aIPAddr+n));
      pENetAddr = ARP_Query(aIPAddr+n);
      if(pENetAddr == (PENETADDR) 0)
        printf("Address not resolved yet.\r\n");
      else if(pENetAddr == (PENETADDR) -1)
        printf("Address could not be resolved.\r\n");
      else
        printf("%s\r\n", ENetAddrToA(pENetAddr));

    }
    else if(c=='r' || c=='R')
    {
      ARP_SimulateReceivedQuery();
    }
    else if(c=='l' || c=='L')
    {
      printf("Contents of the ARP cache:\r\n");
      ARP_PrintAll();
      printf("\r\n>");
    }
  }
}



/****************************************************************************
 *
 * Function : main
 *
 * Purpose  : Call ARP_Init, UserLoop and then ARP_Cleanup
 *
 * Returns  : 0
 *
 ***************************************************************************/

int main(int argc, char *argv[])
{
  ARP_Init();
  UserLoop();
  ARP_Cleanup();
  return 0;
}
