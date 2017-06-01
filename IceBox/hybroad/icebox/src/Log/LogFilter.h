#ifndef _LogFilter_H_
#define _LogFilter_H_

#include <stdint.h>

#ifdef __cplusplus

namespace IBox {

class LogFilter {
public:
    LogFilter();
    ~LogFilter();

    virtual bool pushBlock(uint8_t* blockHead, uint32_t blockLength) = 0;

    LogFilter* m_next; 
};

} // namespace IBox

#endif // __cplusplus

#endif // _LogFilter_H_
