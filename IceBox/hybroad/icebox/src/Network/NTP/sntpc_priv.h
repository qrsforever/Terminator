/* sntpcLib.h - Simple Network Time Protocol client include file */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
Modification history 
--------------------
01c,25aug99,cno  Add extern "C" definition (SPR21825)
01b,15jul97,spm  code cleanup, documentation, and integration; entered in
				 source code control
01a,20apr97,kyc  written

*/

#ifndef __INCsntpch
#define __INCsntpch

/* includes */
#include "sntp.h"

/* defines */
#define M_sntpcLib		(113 << 16)
#define S_sntpcLib_INVALID_PARAMETER		 (M_sntpcLib | 1)
#define S_sntpcLib_INVALID_ADDRESS			 (M_sntpcLib | 2)
#define S_sntpcLib_TIMEOUT					 (M_sntpcLib | 3)
#define S_sntpcLib_VERSION_UNSUPPORTED		 (M_sntpcLib | 4)
#define S_sntpcLib_SERVER_UNSYNC			 (M_sntpcLib | 5)

#define SNTP_CLIENT_REQUEST 0x0B			 /* standard SNTP client request */

#ifdef __cplusplus
extern "C" {
#endif

 int  sntpcInit (unsigned short port );
 int sntpcTimeGet (  char *   pServerAddr,	  unsigned int timeout, struct timespec *	pCurrTime);
#ifdef __cplusplus
}
#endif

#endif /* __INCsntpch */


