#include "NativeHandler.h"
#include "NativeHandlerPublic.h"
#include "NativeHandlerCustomer.h"
#include "NativeHandlerAssertions.h"

#include "Message.h"

namespace IBox {

void 
NativeHandler::handleMessage(Message *msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
}

static NativeHandler::Callback *gCallbacks[NativeHandler::kRecovery + 1] = {0};

int 
NativeHandler::registerStateHandler(State state, Callback *callback)
{
    if (state > kRecovery)
        return -1;

    gCallbacks[state] = callback;
    return 0;
}

int 
NativeHandler::setState(State state)
{
    if (state > kRecovery)
        return -1;
    
    if (mCallback) {
        NativeHandlerPublic *handler = (NativeHandlerPublic *)mCallback;
        if (state == handler->state())
            return 0;
        handler->onUnactive();
        oldState = handler->state();
    } else
        oldState = state;

    // NATIVEHANDLER_LOG("Current stb state(%d)\n", state);
    mCallback = gCallbacks[state];

    if(mCallback) {
        NativeHandlerPublic *handler = (NativeHandlerPublic *)mCallback;
        handler->onActive();
    } else {
		return -1;
	}
    return 0;
}

NativeHandler::State 
NativeHandler::getState()
{
    NativeHandlerPublic *handler = (NativeHandlerPublic *)mCallback;
    if (handler)
        return handler->state();
    else
        return kBoot;
}

NativeHandler::State 
NativeHandler::getOldState()
{
    return oldState;
}

static NativeHandler *gNativeHandler = NULL;

NativeHandler &defNativeHandler()
{
    return *gNativeHandler;
}

void NativeHandlerCreate()
{
    if (!gNativeHandler) {
        gNativeHandler = new NativeHandler();
        NativeHandlerCustomerInit();
    }
}

} // namespace IBox

extern "C" void 
sendMessageToNativeHandler(int what, int arg1, int arg2, unsigned int pDelayMillis)
{
    IBox::Message *msg = IBox::defNativeHandler().obtainMessage(what, arg1, arg2);

    if(pDelayMillis > 0)
        IBox::defNativeHandler().sendMessageDelayed(msg, pDelayMillis);
    else
        IBox::defNativeHandler().sendMessage(msg);
}
