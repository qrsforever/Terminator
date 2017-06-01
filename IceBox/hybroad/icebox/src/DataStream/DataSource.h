#ifndef _DataSource_H_
#define _DataSource_H_

#include <stdint.h>
#include <string>

#ifdef __cplusplus

namespace IBox {

class DataSink;
class RingBuffer;

class DataSource {
public:
    DataSource();
    ~DataSource();

    int setBuffer(RingBuffer* buffer) { m_ringBuffer = buffer; return 0; }
    void setDataSourceAddress(const char* sourceAddress) { m_sourceAddress = sourceAddress; }
    
    virtual bool attachSink(DataSink *sink);
    virtual bool detachSink(DataSink *sink);

    bool tellDataSize(uint32_t size);

    virtual bool start();
    virtual bool stop();
    virtual int pause();
    virtual int resume();
    virtual int fastForward();
    virtual int fastBackward();

protected:
    DataSink* m_dataSink;
    RingBuffer* m_ringBuffer;
    std::string m_sourceAddress;

private:
};

} /* namespace IBox */

#endif /* __cplusplus */

#endif /* _DataSource_H_ */
