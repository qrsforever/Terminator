#include "NativeHandlerAssertions.h"
#include "NativeHandlerRunning.h"

#include "Message.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "StringData.h"

#include "GraphicsLayout.h"
#include "ImageView.h"

namespace IBox {

NativeHandlerRunning::NativeHandlerRunning()
{
}

NativeHandlerRunning::~NativeHandlerRunning()
{
}

void
NativeHandlerRunning::onActive()
{
    NATIVEHANDLER_LOG("\n");
}

void
NativeHandlerRunning::onUnactive()
{
    NATIVEHANDLER_LOG("\n");
}

bool 
NativeHandlerRunning::handleMessage(Message *msg)
{
    switch (msg->what) {
    default:
        break;
    }
    return NativeHandlerPublic::handleMessage(msg);
}

} // namespace IBox
