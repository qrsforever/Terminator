
#include "RingBuffer.h"

#include "PlatBuffer.h"
#include "DataSource.h"

#include <stdio.h>


namespace IBox {

RingBuffer::RingBuffer(PlatBuffer *platBuffer, bool waitLock)
	: mPlatBuffer(platBuffer)
	, mPrestore(0)
	, mWrite(0)
	, mRead(0)
	, mWaitLock(waitLock)
	, mDataSource(NULL)
{
    mBufHead = platBuffer->getBufHead();
    mBufLength = platBuffer->getBufLength();

    if (mWaitLock) {
        pthread_cond_init(&mCond, NULL);
        pthread_mutex_init(&mMutex, NULL);
    }
}

RingBuffer::RingBuffer(uint8_t *bufHead, uint32_t bufLength, bool waitLock)
	: mPlatBuffer(NULL)
	, mBufHead(bufHead)
	, mBufLength(bufLength)
	, mPrestore(0)
	, mWrite(0)
	, mRead(0)
	, mWaitLock(waitLock)
	, mDataSource(NULL)
{
    if (mWaitLock) {
        pthread_cond_init(&mCond, NULL);
        pthread_mutex_init(&mMutex, NULL);
    }
}

RingBuffer::~RingBuffer()
{
	if(mDataSource){
		delete mDataSource;	
	}
	if(mPlatBuffer){
		delete mPlatBuffer;
	}

    if (mWaitLock) {
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCond);
    }
}

bool 
RingBuffer::setDataSource(DataSource *dataSource)
{
    if (mDataSource)
        return false;
    mDataSource = dataSource;
    return true;
}

int 
RingBuffer::getPrestoreHead(uint8_t **bufPointer, uint32_t *bufLength)
{
    return -1;
}

int 
RingBuffer::submitPrestore(uint8_t *bufPointer, uint32_t bufLength)
{
    return -1;
}

int 
RingBuffer::getWriteHead(uint8_t **bufPointer, uint32_t *bufLength)
{
    /* Note: Don't use LogModule function, otherwise cause deathlock, Ref: Log.cpp */
    if (mWaitLock)
        pthread_mutex_lock(&mMutex);
Retry:
    uint32_t bakRead = mRead;
    uint32_t maxWriteLength;
    if (mWrite >= bakRead) {
        maxWriteLength = mBufLength - mWrite;
        if (bakRead == 0)
            maxWriteLength = maxWriteLength - 1;
    }
    else {
        maxWriteLength = (bakRead - 1) - mWrite;
    }

    if (mWaitLock && !maxWriteLength) {
        pthread_cond_wait(&mCond, &mMutex);
        goto Retry;
    }

   	*bufPointer = mBufHead + mWrite;
   	*bufLength = maxWriteLength;
    if (mWaitLock)
        pthread_mutex_unlock(&mMutex);
    return 0;
}

int 
RingBuffer::submitWrite(uint8_t *bufPointer, uint32_t bufLength)
{
    /* Note: Don't use LogModule function, otherwise cause deathlock, Ref: Log.cpp */
    if ((mBufHead + mWrite) != bufPointer)
        return -1;

    uint32_t bakRead = mRead;
    uint32_t maxWriteLength;
    if (mWrite >= bakRead) {
        maxWriteLength = mBufLength - mWrite;
        if (bakRead == 0)
            maxWriteLength = maxWriteLength - 1;
    }
    else {
        maxWriteLength = (bakRead - 1) - mWrite;
    }

    if (maxWriteLength < bufLength)
        return -1;

    mWrite += bufLength;
    if (mWrite == mBufLength)
        mWrite = 0;

    return 0;
}

int 
RingBuffer::getReadHead(uint8_t **bufPointer, uint32_t *bufLength)
{
    uint32_t bakWrite = mWrite;
    uint32_t maxReadLength;
    if (mRead > bakWrite) {
        maxReadLength = mBufLength - mRead;
    }
    else {
        maxReadLength = bakWrite - mRead;
    }

   	*bufPointer = mBufHead + mRead;
   	*bufLength = maxReadLength;

    return 0;
}

int 
RingBuffer::submitRead(uint8_t *bufPointer, uint32_t bufLength)
{
    if ((mBufHead + mRead) != bufPointer)
        return -1;

    if (mWaitLock)
        pthread_mutex_lock(&mMutex);

    uint32_t bakWrite = mWrite;
    uint32_t maxReadLength;
    if (mRead > bakWrite) {
        maxReadLength = mBufLength - mRead;
    }
    else {
        maxReadLength = bakWrite - mRead;
    }

    int ret = -1;
    if (bufLength <= maxReadLength) {
        mRead += bufLength;
        if (mRead == mBufLength)
            mRead = 0;
        ret = 0;
    }

    if (mWaitLock) {
        if (!ret)
            pthread_cond_signal(&mCond);
        pthread_mutex_unlock(&mMutex);
    }
    return ret;
}

} /* namespace IBox */
