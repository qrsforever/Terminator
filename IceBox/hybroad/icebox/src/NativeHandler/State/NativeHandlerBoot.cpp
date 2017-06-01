#include "NativeHandlerAssertions.h"
#include "NativeHandlerBoot.h"

#include "Message.h"
#include "MessageTypes.h"
#include "NativeHandler.h"

#include "GraphicsLayout.h"
#include "ImageView.h"
#include "NetworkManager.h"
#include "NetworkCard.h"
#include "WeatherManager.h"
#include "SysSetting.h"

#include "XmppShell.h"
#include "PromptMessage.h"

void IBoxBootStage(int s);

namespace IBox {

NativeHandlerBoot::NativeHandlerBoot()
    : mBoxLoginOk(false)
    , mNetConnectOk(false)
{
}

NativeHandlerBoot::~NativeHandlerBoot()
{
}

void
NativeHandlerBoot::onActive()
{
    if (mBoxLoginOk)
        return;
    Prompt().addMessageI(ePM_SoftwareVersion, GetMessages(ePM_SoftwareVersion));
    Network().autoSwitch();
}

void
NativeHandlerBoot::onUnactive()
{

}

bool 
NativeHandlerBoot::handleMessage(Message *msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->what) {
    case MessageType_Network:
        switch (msg->arg1) {
        case MV_NETWORK_CONNECT_OK:
            Network().NTPSync().syncStart();
            Network().sendRemotePacket();
            Prompt().addMessageI(ePM_NetworkAddress, std::string("IP: ").append(Network().getAddress()).c_str());
            mNetConnectOk = true;
            XmppShell::self().login();
            WeatherManager::self().query();
            Network().startThunder();
            return true;
        case MV_NETWORK_CONNECT_ERROR:
            Prompt().addMessageW(ePM_NetworkError, GetMessages(ePM_NetworkError));
            mNetConnectOk = false;
            break;
        case MV_NETWORK_NTPSYNC_OK:
            // WeatherManager::self().query();
            // XmppShell::self().login();
            break;
        case MV_NETWORK_NTPSYNC_ERROR:
        case MV_NETWORK_NTPSYNC_TIMEOUT:
            Prompt().addMessageW(ePM_NtpSyncError, GetMessages(ePM_NtpSyncError));
            break;
        case MV_NETWORK_EASYSETUP_START:
            changeState(NativeHandler::kEasysetup);
            return true;
        case MV_NETWORK_JOINAP_OK:
            Network().connectWireless();
            return true;
        case MV_NETWORK_JOINAP_ERROR:
            return true;
        default:
            ;
        }
        break;
    case MessageType_Xmpp:
        switch (msg->arg1) {
        case MV_XMPP_ICEBOX_LOGINOK:
            XmppShell::self().mTryCnt = 0;
            mBoxLoginOk = true;
            Prompt().addMessageI(ePM_BoxNotConnected, GetMessages(ePM_BoxLoginSuccess));
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_CHECK, 0, 0)); 
            changeState(NativeHandler::kRunning);
            return true;
        case MV_XMPP_ICEBOX_LOGINERR:
            if (XmppShell::self().mTryCnt > LOGIN_MAX_TRYCOUNT) {
                Prompt().addMessageE(ePM_BoxNotConnected, GetMessages(ePM_BoxLoginFail));
                XmppShell::self().mTryCnt = 0;
            } else {
                XmppShell::self().login();
            }
            mBoxLoginOk = false;
            return true;
        default:
            ;
        }
        break;
    case MessageType_KeyUp:
        if (MV_PRESSKEY_REGISTUSER == msg->arg1) {
            Prompt().addMessageE(ePM_BoxNotConnected, GetMessages(ePM_BoxNotConnected));
            return true;
        }
    default:
        break;
    }
    return NativeHandlerPublic::handleMessage(msg);
}

} // namespace IBox
