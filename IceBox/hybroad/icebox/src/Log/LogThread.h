#ifndef _LogThread_H_
#define _LogThread_H_

#include "Thread.h"

#ifdef __cplusplus

namespace IBox {

class LogThread : public Thread {
public:
    LogThread();
    ~LogThread();

    virtual void run();
};

void LogThreadInit();

} /* namespace IBox */

#endif // __cplusplus

#endif // _LogThread_H_
