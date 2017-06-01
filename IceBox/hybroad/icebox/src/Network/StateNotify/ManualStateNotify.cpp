#include "NetworkAssertions.h"
#include "ManualStateNotify.h"
#include "NativeHandler.h"
#include "MessageTypes.h"

namespace IBox {

ManualStateNotify::ManualStateNotify(NetworkInterface* iface) 
    : StateNotify(iface)
{
}

ManualStateNotify::~ManualStateNotify()
{
}

int
ManualStateNotify::notify(int state) 
{
    NETWORK_LOG("state:%d\n", state);

    NativeHandler& H = defNativeHandler();
    H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_CONNECT_OK, 0, 0), 2000);  //2s for test boot image
    return 0;
}

}
