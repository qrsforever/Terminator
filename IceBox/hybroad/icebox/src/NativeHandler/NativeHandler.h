#ifndef _NativeHandler_H_
#define _NativeHandler_H_

#include "MessageHandler.h"

#ifdef __cplusplus

namespace IBox { 

class NativeHandler : public MessageHandler {
public:
    virtual void handleMessage(Message *msg);

    enum State {
        kBoot = 0,
        kAuthorize,
        kConfig,
        kRunning,
        kDiskCopy,
        kUpgrade,
        kEasysetup,
        kStandby,
        kRecovery
    };

    static int registerStateHandler(State, Callback *);

    int setState(State);
    State getState();
    State getOldState();  
private:
    State oldState;
};

NativeHandler &defNativeHandler();

void NativeHandlerCreate();

} // namespace IBox 

#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif

void sendMessageToNativeHandler(int what, int arg1, int arg2, unsigned int pDelayMillis);

#ifdef __cplusplus
}
#endif

#endif // _NativeHandler_H_
