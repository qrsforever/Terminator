/* Simple Network Time Protocol (SNTP) client */

#include "Assertions.h"
#include "sntpc_priv.h"

#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SNTP_PORT 123

/* forward declarations */

static int sntpc_listen (struct timeval *ptv_timeout, struct timespec *pts);
static int sntpc_fetch (struct in_addr *pserver_addr, struct timeval *ptv_timeout, struct timespec *pts);

int sntpc_gettime (const char *server, struct timeval *ptv_timeout, struct timespec *pts);

// 返回值
//  0  成功
// -1 常规错误（系统调用等）
// -2 超时
// -3 失败。
int sntpc_gettime (const char *server, struct timeval *ptv_timeout, struct timespec *pts)
{
    struct in_addr target;
    int result;

    if (pts == NULL) {
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    if ((server == NULL) || (server[0] == '\0'))
        result = sntpc_listen (ptv_timeout, pts);
    else {
        target.s_addr = inet_addr (server);

        if (target.s_addr == 0) {
            SYSTEM_LOG_ERROR("\n");
            goto Err;
        }
        result = sntpc_fetch (&target, ptv_timeout, pts);
    }

    return (result);
Err:
    return -1;
}

/*******************************************************************************
 *
 * sntpcFractionToNsec - convert time from the NTP format to POSIX time format
 *
 * This routine converts the fractional part of the NTP timestamp format to a 
 * value in nanoseconds compliant with the POSIX clock.  While the NTP time 
 * format provides a precision of about 200 pico-seconds, rounding error in the 
 * conversion routine reduces the precision to tenths of a micro-second.
 * 
 * RETURNS: Value for struct timespec corresponding to NTP fractional part
 *
 * ERRNO:   N/A
 *
 * INTERNAL
 *
 * Floating-point calculations can't be used because some boards (notably
 * the SPARC architectures) disable software floating point by default to 
 * speed up context switching. These boards abort with an exception when
 * floating point operations are encountered.
 *
 * NOMANUAL
 */

/*
 *
 *//* base 2 fractional part of the NTP timestamp */
static uint32_t sntpcFractionToNsec (uint32_t sntpFraction)
{
    uint32_t factor = 0x8AC72305;	/* Conversion factor from base 2 to base 10 */
    uint32_t divisor = 10;			/* Initial exponent for mantissa. */
    uint32_t mask = 1000000000;	/* Pulls digits of factor from left to right. */
    int loop;
    uint32_t nsec = 0;
    int shift = 0;			/* Shifted to avoid overflow? */


    /* 
     * Adjust large values so that no intermediate calculation exceeds 
     * 32 bits. (This test is overkill, since the fourth MSB can be set 
     * sometimes, but it's fast).
     */

    if (sntpFraction & 0xF0000000) {
        sntpFraction /= 10;
        shift = 1;
    }

    /* 
     * In order to increase portability, the following conversion avoids
     * floating point operations, so it is somewhat obscure.
     *
     * Incrementing the NTP fractional part increases the corresponding
     * decimal value by 2^(-32). By interpreting the fractional part as an
     * integer representing the number of increments, the equivalent decimal
     * value is equal to the product of the fractional part and 0.2328306437.
     * That value is the mantissa for 2^(-32). Multiplying by 2.328306437E-10
     * would convert the NTP fractional part into the equivalent in seconds.
     *
     * The mask variable selects each digit from the factor sequentially, and
     * the divisor shifts the digit the appropriate number of decimal places. 
     * The initial value of the divisor is 10 instead of 1E10 so that the 
     * conversion produces results in nanoseconds, as required by POSIX clocks.
     */

    for (loop = 0; loop < 10; loop++) {	/* Ten digits in mantissa */
        nsec += sntpFraction * (factor / mask) / divisor;	/* Use current digit. */
        factor %= mask;			/* Remove most significant digit from the factor. */
        mask /= 10;				/* Reduce length of mask by one. */
        divisor *= 10;			/* Increase preceding zeroes by one. */
    }

    /* Scale result upwards if value was adjusted before processing. */

    if (shift)
        nsec *= 10;

    return (nsec);
}

// 返回值
// 0  成功
// -1 常规错误（系统调用等）
// -2 超时
// -3 失败。
static int sntpc_fetch (struct in_addr *pserver_addr, struct timeval *ptv_timeout, struct timespec *pts)
{
    SNTP_PACKET sntpRequest;	/* sntp request packet for */
    /* transmission to server */
    SNTP_PACKET sntpReply;		/* buffer for server reply */
    struct sockaddr_in dstAddr;
    struct sockaddr_in servAddr;
    int optval;
    int sntpSocket;
    fd_set readFds;
    int result;
    size_t servAddrLen;

    /* Set destination for request. */

    bzero ((char *) &dstAddr, sizeof (dstAddr));
    dstAddr.sin_addr.s_addr = pserver_addr->s_addr;
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons (SNTP_PORT);

    /* Create socket for transmission. */

    sntpSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (sntpSocket == -1) {
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    optval = 1;
    result = setsockopt (sntpSocket, SOL_SOCKET, SO_BROADCAST, (char *) &optval, sizeof (optval));

    /* Initialize SNTP message buffers. */

    bzero ((char *) &sntpRequest, sizeof (sntpRequest));
    bzero ((char *) &sntpReply, sizeof (sntpReply));

    sntpRequest.leapVerMode = SNTP_CLIENT_REQUEST;

    bzero ((char *) &servAddr, sizeof (servAddr));
    servAddrLen = sizeof (servAddr);

    /* Transmit SNTP request. */

    if (sendto (sntpSocket, (caddr_t) & sntpRequest, sizeof (sntpRequest), 0,
            (struct sockaddr *) &dstAddr, sizeof (dstAddr)) == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    FD_ZERO (&readFds);
    FD_SET (sntpSocket, &readFds);

    result = select (sntpSocket + 1, &readFds, NULL, NULL, ptv_timeout);
    if (result == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }
    if (result == 0) {
        close (sntpSocket);
        SYSTEM_LOG("sntpc fetch timedout.\n");
        return -2;
    }

    result = recvfrom (sntpSocket, (caddr_t)&sntpReply, sizeof(sntpReply),
        0, (struct sockaddr *)&servAddr, &servAddrLen);
    if (result == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }
    close (sntpSocket);

    /*
     * Return error if the server clock is unsynchronized, or the version is 
     * not supported.
     */

    // trying to resolve time if LI_MASK is 3.
#if 0
    if ((sntpReply.leapVerMode & SNTP_LI_MASK) == SNTP_LI_3 || sntpReply.transmitTimestampSec == 0) {
        SYSTEM_LOG("%d, %d\n", (sntpReply.leapVerMode & SNTP_LI_MASK), sntpReply.transmitTimestampSec);
        return -3;
    }
#endif

    if ((sntpReply.leapVerMode & SNTP_VN_MASK) == SNTP_VN_0 || (sntpReply.leapVerMode & SNTP_VN_MASK) > SNTP_VN_3) {
        SYSTEM_LOG("sntp reply VN_MASK > SNTP_VN_3\n");
        return -3;
    }

    /* Convert the NTP timestamp to the correct format and store in clock. */

    /* Add test for 2036 base value here! */

    sntpReply.transmitTimestampSec = ntohl (sntpReply.transmitTimestampSec) - SNTP_UNIX_OFFSET;

    /* 
     * Adjust returned value if leap seconds are present. 
     * This needs work! 
     */

    /*if ((sntpReply.leapVerMode & SNTP_LI_MASK) == SNTP_LI_1)
      sntpReply.transmitTimestampSec += 1;
      else if ((sntpReply.leapVerMode & SNTP_LI_MASK) == SNTP_LI_2)
      sntpReply.transmitTimestampSec -= 1;
      */

    sntpReply.transmitTimestampFrac = ntohl (sntpReply.transmitTimestampFrac);

    pts->tv_sec = sntpReply.transmitTimestampSec;
    pts->tv_nsec = sntpcFractionToNsec (sntpReply.transmitTimestampFrac);

    return 0;
Err:
    return -1;
}

static int sntpc_listen (struct timeval *ptv_timeout, struct timespec *pts)
{
    SNTP_PACKET sntpMessage;	/* buffer for message from server */
    struct sockaddr_in srcAddr;
    int sntpSocket;
    fd_set readFds;
    int result;
    size_t srcAddrLen;

    /* Initialize source address. */

    bzero ((char *) &srcAddr, sizeof (srcAddr));
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = INADDR_ANY;
    srcAddr.sin_port = htons (SNTP_PORT);

    /* Create socket for listening. */

    sntpSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (sntpSocket == -1) {
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    result = bind (sntpSocket, (struct sockaddr *) &srcAddr, sizeof (srcAddr));
    if (result == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    /* Wait for broadcast message from server. */

    FD_ZERO (&readFds);
    FD_SET (sntpSocket, &readFds);

    result = select (FD_SETSIZE, &readFds, NULL, NULL, ptv_timeout);
    if (result == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }
    if (result == 0) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    result = recvfrom (sntpSocket, (caddr_t) & sntpMessage, sizeof (sntpMessage),
        0, (struct sockaddr *) &srcAddr, &srcAddrLen);
    if (result == -1) {
        close (sntpSocket);
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }
    close (sntpSocket);

    /*
     * Return error if the server clock is unsynchronized, or the version is 
     * not supported.
     */

    if ((sntpMessage.leapVerMode & SNTP_LI_MASK) == SNTP_LI_3 || sntpMessage.transmitTimestampSec == 0) {
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    if ((sntpMessage.leapVerMode & SNTP_VN_MASK) == SNTP_VN_0
        || (sntpMessage.leapVerMode & SNTP_VN_MASK) > SNTP_VN_3) {
        SYSTEM_LOG_ERROR("\n");
        goto Err;
    }

    /* Convert the NTP timestamp to the correct format and store in clock. */

    /* Add test for 2036 base value here! */

    sntpMessage.transmitTimestampSec = ntohl (sntpMessage.transmitTimestampSec) - SNTP_UNIX_OFFSET;

    /* 
     * Adjust returned value if leap seconds are present. 
     * This needs work! 
     */

    /*if ((sntpMessage.leapVerMode & SNTP_LI_MASK) == SNTP_LI_1)
      sntpMessage.transmitTimestampSec += 1;
      else if ((sntpMessage.leapVerMode & SNTP_LI_MASK) == SNTP_LI_2)
      sntpMessage.transmitTimestampSec -= 1;
      */

    sntpMessage.transmitTimestampFrac = ntohl (sntpMessage.transmitTimestampFrac);

    pts->tv_sec = sntpMessage.transmitTimestampSec;
    pts->tv_nsec = sntpcFractionToNsec (sntpMessage.transmitTimestampFrac);

    return 0;
Err:
    return -1;
}
