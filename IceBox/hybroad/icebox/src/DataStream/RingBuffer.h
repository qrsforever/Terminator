#ifndef _RingBuffer_H_
#define _RingBuffer_H_

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus

namespace IBox {

class PlatBuffer;
class DataSource;

class RingBuffer {
public:
    RingBuffer(PlatBuffer *platBuffer, bool waitLock = false);
    RingBuffer(uint8_t *bufHead, uint32_t bufLength, bool waitLock = false);
    ~RingBuffer();

    bool setDataSource(DataSource *dataSource);
    DataSource *getDataSource() { return mDataSource; }

    int getPrestoreHead(uint8_t **bufPointer, uint32_t *bufLength);
    int submitPrestore(uint8_t *bufPointer, uint32_t bufLength);

    int getWriteHead(uint8_t **bufPointer, uint32_t *bufLength);
    int submitWrite(uint8_t *bufPointer, uint32_t bufLength);

    int getReadHead(uint8_t **bufPointer, uint32_t *bufLength);
    int submitRead(uint8_t *bufPointer, uint32_t bufLength);

private:
    PlatBuffer *mPlatBuffer;
    uint8_t    *mBufHead;
    uint32_t    mBufLength;
    uint32_t    mPrestore;
    uint32_t    mWrite;
    uint32_t    mRead;

    bool mWaitLock;
    pthread_cond_t mCond;
    pthread_mutex_t mMutex;

    DataSource *mDataSource;
};

} /* namespace IBox */

#endif /* __cplusplus */

#endif /* _RingBuffer_H_ */
