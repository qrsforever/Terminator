/* sntp.h - Simple Network Time Protocol common include file */


#ifndef __INCsntph
#define __INCsntph

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/types.h"

/* defines */

/* constants used by the NTP packet. See RFC 1769 for details. */

/* 2 bit leap indicator field */

#define SNTP_LI_MASK        0xC0
#define SNTP_LI_0           0x00           /* no warning  */
#define SNTP_LI_1           0x40           /* last minute has 61 seconds */
#define SNTP_LI_2           0x80           /* last minute has 59 seconds */
#define SNTP_LI_3           0xC0           /* alarm condition 
					      (clock not synchronized) */

/* 3 bit version number field */

#define SNTP_VN_MASK        0x38
#define SNTP_VN_0           0x00           /* not supported */
#define SNTP_VN_1           0x08           /* the earliest version */
#define SNTP_VN_2           0x10      
#define SNTP_VN_3           0x18           /* VxWorks implements this 
					      version */
#define SNTP_VN_4           0x20           /* the latest version, not yet 
					      solidified */
#define SNTP_VN_5           0x28           /* reserved */
#define SNTP_VN_6           0x30           /* reserved */
#define SNTP_VN_7           0x38           /* reserved */

/* 3 bit mode field */

#define SNTP_MODE_MASK      0x07      
#define SNTP_MODE_0         0x00           /* reserve */
#define SNTP_MODE_1         0x01           /* symmetric active */
#define SNTP_MODE_2         0x02           /* symmetric passive */
#define SNTP_MODE_3         0x03           /* client */
#define SNTP_MODE_4         0x04           /* server */
#define SNTP_MODE_5         0x05           /* broadcast */
#define SNTP_MODE_6         0x06           /* reserve for NTP control 
					      message */
#define SNTP_MODE_7         0x07           /* reserve for private use */



/* 8 bit stratum number. Only the first 2 are valid for SNTP. */

#define SNTP_STRATUM_0      0x00           /* unspecified or unavailable */
#define SNTP_STRATUM_1      0x01           /* primary source */

/* 
 * No default constants are defined for poll, precision, root delay, 
 * root dispersion and reference identifier. Users are expected to supply 
 * values for the poll interval and the refererence identifier. SNTP ignores 
 * the precision, root delay and root dispersion fields.
 */


/*
 * Time conversion constant. NTP timestamps are relative to
 * 0h on 1 January 1900, Unix uses 0h GMT on 1 January 1970 as a base.
 * The defined constant incorporates 53 standard years and 17 leap years,
 * but omits all leap second adjustments, since these are applied to
 * both timescales, keeping the offset constant.
 */

#define SNTP_UNIX_OFFSET  0x83aa7e80    /* 1970 - 1900 in seconds */

/* the range of the struct timeval is 0 - 1000000 */

#define TIMEVAL_USEC_MAX 1000000


/* 
 * SNTP_PACKET - Network Time Protocol message format (without authentication).
 *               See RFC1769 for details. 
 */


typedef struct sntpPacket
    {
    unsigned char     leapVerMode;
    unsigned char     stratum;                 
    char              poll;
    char              precision;
    u_long            rootDelay;
    u_long            rootDispersion;
    u_long            referenceIdentifier;

    /* latest available time, in 64-bit NTP timestamp format */

    u_long            referenceTimestampSec;
    u_long            referenceTimestampFrac;

    /* client transmission time, in 64-bit NTP timestamp format */

    u_long            originateTimestampSec;
    u_long            originateTimestampFrac;

    /* server reception time, in 64-bit NTP timestamp format */

    u_long            receiveTimestampSec;
    u_long            receiveTimestampFrac;

    /* server transmission time, in 64-bit NTP timestamp format */

    u_long            transmitTimestampSec;
    u_long            transmitTimestampFrac;
    } SNTP_PACKET;


#ifdef __cplusplus
}
#endif
#endif /* __INCsntph */
