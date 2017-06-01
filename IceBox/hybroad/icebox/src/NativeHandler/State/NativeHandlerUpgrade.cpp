#include "NativeHandlerAssertions.h"
#include "NativeHandlerUpgrade.h"

#include "GraphicsLayout.h"
#include "Message.h"
#include "MessageTypes.h"
#include "NativeHandler.h"
#include "XmppShell.h"
#include "UpgradeManager.h"
#include "UpgradeView.h"
#include "UpgradeConstant.h"
#include "Setting.h"
#include "SysSetting.h"
#include "PlatformAPI.h"
#include "PromptMessage.h"

namespace IBox {

NativeHandlerUpgrade::NativeHandlerUpgrade()
{
}

NativeHandlerUpgrade::~NativeHandlerUpgrade()
{
}

void
NativeHandlerUpgrade::onActive()
{
    const char* version = UpgradeManager::self().getNewVersionNumber();
    bool isLocal = (UpgradeManager::self().type() == UpgradeManager::eUT_LOCAL);
    char buffer[128] = {0};
    sysSetting().get("IceBox.UpgradeAuto", buffer, 127);
    if (!isLocal && !strncasecmp(buffer, "on", 2)) {
        //server auto upgrade
        int res = UpgradeManager::self().start();
        if (UPGRADE_START != res)
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, -1, 0)); 
    } else {
        //prompt
        Desktop().showUpgradeLayer();
        Desktop().getUpgradeView()->promptVersionMessage(version, isLocal).inval(0);
        Prompt().addMessageX(ePM_UpgradeTimingDown, 30);
    }
}

void
NativeHandlerUpgrade::onUnactive()
{
    Desktop().hideUpgradeLayer();
    Prompt().delMessage(ePM_UpgradeTimingDown);
}

bool 
NativeHandlerUpgrade::handleMessage(Message *msg)
{
    // NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    int res = -1;
    switch (msg->what) {
    case MessageType_KeyUp:
        if (msg->arg1 == MV_PRESSKEY_SHORTFP) {
            //timer count down
            if (Prompt().getCurrentMsgID() == ePM_UpgradeTimingDown) {
                Prompt().delMessage(ePM_UpgradeTimingDown);
                res = UpgradeManager::self().start();
                if (UPGRADE_START != res)
                    H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, -1, 0)); 
                return true;
            }
            //system reboot prompt
            if (Prompt().getCurrentMsgID() == ePM_RebootAsk) {
                Prompt().delMessage(ePM_RebootAsk);
                Platform().reboot();
                return true;
            }
            return true;
        } 
        return false;
    case MessageType_Disk:
        //delay send disk message.
        return onDelay(msg);
    case MessageType_Upgrade:
        switch (msg->arg1) {
         case MV_UPGRADE_REQSOFTWARE:
            if (msg->arg2 < 0) {
                //download err
                if (msg->arg2 == -1)
                    Prompt().addMessageE(ePM_UDownloadFail, GetMessages(ePM_UDownloadFail));
                else 
                    Prompt().addMessageE(ePM_UVerifyFail, GetMessages(ePM_UVerifyFail));
                H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, -1, 0)); 
                return true;
            }
            //download ok
            Prompt().addMessageI(ePM_UDownloadSuccess, GetMessages(ePM_UDownloadSuccess));
            res = UpgradeManager::self().burning();
            if (UPGRADE_BURNING != res)
                H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, -1, 0)); 
            return true;
        case MV_UPGRADE_DPROGRESS:
            Desktop().getUpgradeView()->setDownloadProgress(msg->arg2).inval(0);
            return true;
        case MV_UPGRADE_BURNING:
            if (msg->arg2 < 0) {
                //burning err
                Prompt().addMessageE(ePM_UBurningFail, GetMessages(ePM_UBurningFail));
                H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, -1, 0)); 
                return true;
            }
            //burning success, 30s reboot prompt
            Desktop().getUpgradeView()->promptSuccessMessage().inval(0);
            Prompt().addMessageI(ePM_RebootAsk, GetMessages(ePM_RebootAsk), 0, 30);
            return true;
        case MV_UPGRADE_BPROGRESS:
            Desktop().getUpgradeView()->setBurningProgress(msg->arg2).inval(0);
            return true;
        case MV_UPGRADE_END:
            revertState();
            break;
        default:
            ;
        }
        break;
    case MessageType_Prompt:
        switch (msg->arg1) {
        case MV_PROMTP_TIMEOUT:
            if (msg->arg2 == ePM_UpgradeTimingDown) {
                H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, 1, 0)); 
                return true;
            }
            if (msg->arg2 == ePM_RebootAsk) {
                H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, 2, 0)); 
                return true;
            }
            return true;
        default:
            ;
        }
        break;
    }
    return NativeHandlerPublic::handleMessage(msg);
}

} // namespace IBox
