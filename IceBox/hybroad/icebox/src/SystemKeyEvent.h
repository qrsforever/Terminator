#ifndef _SystemKeyEvent_H_
#define _SystemKeyEvent_H_

#include "Thread.h"
#include "SysTime.h"
#include "libzebra.h"
#include "NativeHandler.h"

#ifdef __cplusplus

namespace IBox {

class SystemKeyEvent : public Thread {
private:
    friend void SystemKeyEventInit();
    SystemKeyEvent();

public:
    ~SystemKeyEvent();

    void translate();

    void onHotplug();
    void onMountDisk();
    void onPressKey();

    virtual void run();

private:
    unsigned int mClockTime;
    unsigned int mPressKey;
    InputEvent mEvent;
    NativeHandler& H;
};

void SystemKeyEventInit();

} /* namespace IBox */

#endif // __cplusplus

#endif // _SystemKeyEvent_H_
