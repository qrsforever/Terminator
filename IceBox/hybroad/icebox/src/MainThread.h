#ifndef _MAINTHREAD_H_
#define _MAINTHREAD_H_

#include "Thread.h"

#ifdef __cplusplus

namespace IBox {

class MainThread : public Thread {
public:
    MainThread();
    ~MainThread();
};

void MainThreadInit();
void MainThreadRun();

} /* namespace IBox */

#endif // __cplusplus

#endif /* _MAINTHREAD_H_ */
