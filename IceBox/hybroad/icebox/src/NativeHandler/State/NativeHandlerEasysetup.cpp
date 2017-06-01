#include "NativeHandlerAssertions.h"
#include "NativeHandlerEasysetup.h"

#include "Message.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "StringData.h"

#include "NetworkManager.h"
#include "EasysetupView.h"
#include "GraphicsLayout.h"
#include "RandNumber.h"
#include "PromptMessage.h"

namespace IBox {

NativeHandlerEasysetup::NativeHandlerEasysetup() 
    : mEasysetupView(0), mIDCode("")
    , mCurrentStatus(eCS_Unknow)
{
}

NativeHandlerEasysetup::~NativeHandlerEasysetup()
{
}

void
NativeHandlerEasysetup::onActive()
{
    if (!mEasysetupView) {
        mIDCode = RandNumber::createString(100000, 999999, " ");
        Desktop().showEasysetupLayer();
        mEasysetupView = Desktop().getEasysetupView();
        H.sendMessage(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_QUERING, 0, 0)); 
    }
}

void
NativeHandlerEasysetup::onUnactive()
{
    Desktop().clearEasysetupLayer();
    mEasysetupView = 0;
}

bool 
NativeHandlerEasysetup::handleMessage(Message *msg)
{
    if (!mEasysetupView)
        return false;

    switch (msg->what) {
    case MessageType_Prompt:
        if (MV_PROMTP_TIMEOUT == msg->arg1) {
            switch (msg->arg2) {
            case ePM_EasysetFailtry:
            case ePM_EasysetSuccess:
                Network().startEasySetup();
                H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_QUERING, 0, 0), 1000); 
                break;
            case ePM_PowerOffAsk:
                if (mCurrentStatus == eCS_Interrupt)
                    H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_QUERING, 0, 0), 1000); 
                break;
            }
            return true;
        }
        break;
    case MessageType_KeyDown:
        H.sendMessage(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_INTERRUPT, 0, 0));
        return true;
        break;
    case MessageType_KeyUp:
        if (MV_PRESSKEY_SHORTFP == msg->arg1) {
            switch (Prompt().getCurrentMsgID()) {
            case ePM_PowerOffAsk:
                break;
            case ePM_EasysetFailtry:
                Prompt().delMessage(ePM_EasysetFailtry);
                Network().startEasySetup();
                H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_QUERING, 0, 0), 1000); 
                return true;
            case ePM_EasysetSuccess:
                Prompt().delMessage(ePM_EasysetSuccess);
                Network().closeEasySetup(1);
                revertState();
                return true;
            default:
                return true;
            }
        }
        break;
    case MessageType_Upgrade:
    case MessageType_Disk:
        return false;
    default:
        ;
    }
    return NativeHandlerPublic::handleMessage(msg);
}

bool 
NativeHandlerEasysetup::doNetworkMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    switch (msg->arg1) {
    case MV_NETWORK_EASYSETUP_OK:
        H.removeMessages(MessageType_Network);
        if (msg->obj) {
            mEasysetupView->promptSuccessMessage((char*)(dynamic_cast<StringData*>(msg->obj)->getData()));
            Prompt().delMessage(ePM_EasysetIDCode);
            Prompt().addMessageX(ePM_EasysetSuccess, 30);
        }
        mCurrentStatus = eCS_Success;
        break;
    case MV_NETWORK_EASYSETUP_ERROR:
    case MV_NETWORK_EASYSETUP_TIMEOUT:
        H.removeMessages(MessageType_Network);
        mEasysetupView->promptFailMessage(msg->arg2);
        Prompt().delMessage(ePM_EasysetIDCode);
        Prompt().addMessageX(ePM_EasysetFailtry, 30);
        mCurrentStatus = eCS_Fail;
        break;
    case MV_NETWORK_EASYSETUP_INTERRUPT:
        H.removeMessages(MessageType_Network);
        mEasysetupView->promptInterruptMessage();
        Prompt().delMessage(ePM_EasysetIDCode);
        Prompt().delMessage(ePM_EasysetFailtry);
        mCurrentStatus = eCS_Interrupt;
        break;
    case MV_NETWORK_EASYSETUP_QUERING:
        mEasysetupView->promptQueryMessage(msg->arg2);
        Prompt().addMessageI(ePM_EasysetIDCode, mIDCode.c_str(), 0, -1);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_QUERING, (msg->arg2 + 1), 0), 1000); 
        mCurrentStatus = eCS_Querying;
        break;
    default:
        ;
    }
    return true;
}

} // namespace IBox
