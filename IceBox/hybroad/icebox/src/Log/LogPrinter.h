#ifndef _LogPrinter_H_
#define _LogPrinter_H_

#include "LogFilter.h"

#ifdef __cplusplus

namespace IBox {

class LogPrinter : public LogFilter {
public:
    virtual bool pushBlock(uint8_t* blockHead, uint32_t blockLength);
};

} // namespace IBox

#endif // __cplusplus
#endif // _LogPrinter_H_
