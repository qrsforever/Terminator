#ifndef _DataSink_H_
#define _DataSink_H_

#include <stdint.h>

#ifdef __cplusplus

namespace IBox {

class DataFilter;
class DataSource;
class RingBuffer;

class DataSink {
public:
    DataSink();
    ~DataSink();

    int setBuffer(RingBuffer* buffer) { m_ringBuffer = buffer; return 0; }
    int setDataSize(uint32_t size)  { m_dataSize = size; return 0; }

    virtual bool onStart();
    virtual bool onDataArrive();
    virtual bool onError();
    virtual bool onEnd();
/*
    virtual bool start();
    virtual bool stop();
    virtual bool pause();
    virtual bool resume();

    virtual bool put(void *data, int len);

    virtual void onAttach(DataSource *source);
    virtual void onDetach(DataSource *source);

    virtual void onSourceStart(DataSource *source);
    virtual void onSourceStop(DataSource *source);
    virtual void onSourcePause(DataSource *source);
    virtual void onSourceResume(DataSource *source);
    virtual void onSourceFastForward(DataSource *source);
    virtual void onSourceFastBackward(DataSource *source);

    virtual bool setFilter(DataFilter *filter);
*/
protected:
    RingBuffer* m_ringBuffer;
    uint32_t m_dataSize;

private:
};

} /* namespace IBox */

#endif /* __cplusplus */

#endif /* _DataSink_H_ */
