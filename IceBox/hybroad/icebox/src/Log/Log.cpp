#include "Log.h"

#include "DataSink.h"
#include "RingBuffer.h"
#include "SysTime.h"

#include <string.h>

namespace IBox {

#define MAX_BLOCK_SIZE	256

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* textLevel[] = {"Assert : ", "Error! : ", "Warning: ", "Normal : ", "Verbose: "};
static const char* monthStr[] = {"Undefined", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

Log::Log()
    : mExtStyle(1)
{
}

Log::~Log()
{
}

void Log::log(int level, const char* fmt, va_list args)
{
    uint8_t* bufPointer;
    uint32_t bufLength;

    pthread_mutex_lock(&g_mutex);

    m_ringBuffer->getWriteHead(&bufPointer, &bufLength);
    if (bufLength == 0) {
        pthread_mutex_unlock(&g_mutex);
        return;
    }

    int dataSize = 8, blockSize = 0, writeLen = 0;

    writeLen = snprintf((char*)bufPointer + dataSize, bufLength - dataSize, "%s", textLevel[level]);
    if (writeLen < 0) /* Error. */
        goto Exit;
    dataSize += writeLen;

    writeLen = vsnprintf((char*)bufPointer + dataSize, bufLength - dataSize, fmt, args);
    if (writeLen < 0) /* Error. */
        goto Exit;
    if (writeLen >= ((int)bufLength - dataSize))
        dataSize = bufLength;
    else
        dataSize += writeLen;

    //date size.
    *((int *)(bufPointer + 4)) = dataSize - 8;
    //block size.
    blockSize = (dataSize + 8 + 3) & 0xfffffffc;
    if ((bufLength - blockSize) > MAX_BLOCK_SIZE) {
        *((int *)(bufPointer + 0)) = blockSize;
        m_ringBuffer->submitWrite(bufPointer, blockSize);
    }
    else {
        *((int *)(bufPointer + 0)) = bufLength;
        m_ringBuffer->submitWrite(bufPointer, bufLength);
    }

    if (m_dataSink)
        m_dataSink->onDataArrive();
Exit:
    pthread_mutex_unlock(&g_mutex);
}

void Log::logVerbose(const char* file, int line, const char* function, LogType pType, int level, const char* fmt, va_list args)
{
    uint8_t* bufPointer;
    uint32_t bufLength;

    pthread_mutex_lock(&g_mutex);

    m_ringBuffer->getWriteHead(&bufPointer, &bufLength);  /** getWriteHead cannot use LogModule */
    if (bufLength == 0) {
        pthread_mutex_unlock(&g_mutex);
        return;
    }

    int dataSize = 8, blockSize, writeLen;

    writeLen = logPrefix((char*)bufPointer + dataSize, bufLength - dataSize, file, line, function, pType, level);
    if ((writeLen < 0) || (writeLen >= ((int)bufLength - dataSize))) /* Error or log too long. */
        goto Exit;
    dataSize += writeLen;

    writeLen = vsnprintf((char*)bufPointer + dataSize, bufLength - dataSize, fmt, args);
    if (writeLen < 0) /* Error. */
        goto Exit;
    if (writeLen >= ((int)bufLength - dataSize))
        dataSize = bufLength;
    else
        dataSize += writeLen;

    //date size.
    *((int *)(bufPointer + 4)) = dataSize - 8;
    //block size.
    blockSize = (dataSize + 8 + 3) & 0xfffffffc;
    if ((bufLength - blockSize) > MAX_BLOCK_SIZE) {
        *((int *)(bufPointer + 0)) = blockSize;
        m_ringBuffer->submitWrite(bufPointer, blockSize); /** submitWrite cannot use LogModule */

    }
    else {
        *((int *)(bufPointer + 0)) = bufLength;
        m_ringBuffer->submitWrite(bufPointer, bufLength);
    }
    if (m_dataSink)
        m_dataSink->onDataArrive();
Exit:
    pthread_mutex_unlock(&g_mutex);
}

int Log::logPrefix(char* buffer, int length, const char* file, int line, const char* function, LogType pType, int level)
{
    if (!buffer || !file)
        return -1;
    static SysTime::DateTime sDTime;
    SysTime::GetDateTime(&sDTime);

    const char* pFile = strrchr(file, '/');
    if (pFile)
        pFile = pFile + 1;
    else
        pFile = file;

    static struct timespec sTimeSpec;
    clock_gettime(CLOCK_REALTIME, &sTimeSpec);
    return snprintf(buffer, length, "%02d:%02d:%02d.%03d | %s:%d | %s | %s",
        sDTime.mHour, sDTime.mMinute, sDTime.mSecond, (int)sTimeSpec.tv_nsec / 1000000,  pFile, line, function, textLevel[level]);
}

} // namespace IBox
