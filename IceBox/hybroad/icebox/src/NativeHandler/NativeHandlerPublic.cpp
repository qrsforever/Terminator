#include "NativeHandlerAssertions.h"
#include "NativeHandlerPublic.h"
#include "NativeHandlerCustomer.h"
#include "LCDScreen.h"
#include "SystemManager.h"
#include "SystemKeyEvent.h"

#include "NativeHandler.h"
#include "Message.h"
#include "MessageTypes.h"

#include "FileExplorerThread.h"
#include "XmppShell.h"
#include "StringData.h"
#include "JsonObject.h"

#include "GraphicsLayout.h"
#include "NetworkManager.h"
#include "WeatherManager.h"
#include "DiskDeviceManager.h"
#include "PlatformAPI.h"
#include "UpgradeConstant.h"
#include "UpgradeManager.h"
#include "UpgradeView.h"
#include "UserManager.h"
#include "Setting.h"
#include "SysSetting.h"
#include "RandNumber.h"
#include "DiskCopyView.h"
#include "DiskCopyQueue.h"

#include "PromptMessage.h"
#include "ThunderService.h"

namespace IBox {

NativeHandlerPublic::NativeHandlerPublic()
    : H(defNativeHandler())
{
}

NativeHandlerPublic::~NativeHandlerPublic()
{
}

bool
NativeHandlerPublic::onDelay(Message*msg, int ms)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    return H.sendMessageDelayed(H.obtainMessage(msg->what, msg->arg1, msg->arg2, msg->obj), ms); 
}

bool
NativeHandlerPublic::handleMessage(Message *msg)
{
    // if (msg->what != MessageType_DesktopRefresh && msg->what != MessageType_Repaint)
        // NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    if (msg->what == MessageType_Repaint) {
        systemManager().mixer().refresh(msg->arg1);
        return true;
    }

    switch (msg->what) {
    case MessageType_System:
        return doSystemMessage(msg);
    case MessageType_KeyDown:
        return doKeyDownMessage(msg);
    case MessageType_KeyUp:
        return doKeyUpMessage(msg);
    case MessageType_Network:
        return doNetworkMessage(msg);
    case MessageType_Weather: 
        return doWeatherMessage(msg);
    case MessageType_Xmpp:
        return doXmppMessage(msg);
    case MessageType_Database:
        return doDatabaseMessage(msg);
    case MessageType_Upgrade:
        return doUpgradeMessage(msg);
    case MessageType_Prompt:
        return doPromptMessage(msg);
    case MessageType_Disk:
        return doDiskMessage(msg);
    case MessageType_DesktopRefresh:
        Desktop().refreshAllWidgets();
        H.sendMessageDelayed(H.obtainMessage(MessageType_DesktopRefresh, 0, 0, 0), 1000); 
        break;
    default:
        return false;
    }
    return true;
}

bool 
NativeHandlerPublic::doSystemMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->arg1) {
    case MV_SYSTEM_POWER_DOWN:
        break;
    case MV_SYSTEM_UPS_CHARGE: //arg2 = 1 or 2
        switch (msg->arg2) {
        case 1: //charge full
            UPSWidget().setChargeStatus(UPS_CHARGE_FULL);
            break;
        case 2: //over charge
            UPSWidget().setChargeStatus(UPS_CHARGE_OVER);
            break;
        default:
            return false;
        }
        UPSWidget().setRefreshFlag(REFRESH_FORCE_ONCE);
        break;
    case MV_SYSTEM_HDISK0_DOOR: //arg2 = 0 or 1
        break;
    case MV_SYSTEM_HDISK1_DOOR: //arg2 = 0 or 1
        break;
    default:
        return false;
    }
    return true;
}

bool 
NativeHandlerPublic::doKeyDownMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    switch (msg->arg1) {
    case MV_PRESSKEY_SHORTFP:
        Prompt().delMessage(ePM_KeyPressFP);
        break;
    case MV_PRESSKEY_REGISTUSER:
        Prompt().addMessageI(ePM_KeyPressFP, FP_REGISTER, 0, 1);
        break;
    case MV_PRESSKEY_POWER:
        Prompt().addMessageI(ePM_KeyPressFP, FP_POWEROFF, 0, 1);
        break;
    case MV_PRESSKEY_RESET:
        Prompt().addMessageI(ePM_KeyPressFP, FP_RESET, 0, 1);
        break;
    case MV_PRESSKEY_UNDEF:
        Prompt().addMessageI(ePM_KeyPressFP, FP_UNDEF, 0, 1);
        break;
    default:
        ;
    }
    return true;
}

bool 
NativeHandlerPublic::doKeyUpMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    switch (msg->arg1) {
    case MV_PRESSKEY_REGISTUSER:
        Prompt().delMessage(ePM_KeyPressFP);
        Prompt().addMessageI(ePM_XmppRegisterAsk, GetMessages(ePM_XmppRegisterAsk), 0, 6);
        break;
    case MV_PRESSKEY_POWER:
        Prompt().delMessage(ePM_KeyPressFP);
        Prompt().addMessageI(ePM_PowerOffAsk, GetMessages(ePM_PowerOffAsk), 0, 6);
        break;
    case MV_PRESSKEY_RESET:
        Prompt().delMessage(ePM_KeyPressFP);
        Prompt().addMessageI(ePM_ConfResetAsk, GetMessages(ePM_ConfResetAsk), 0, 6);
        break;
    case MV_PRESSKEY_UNDEF:
        Prompt().delMessage(ePM_KeyPressFP);
        Prompt().addMessageI(ePM_DebugAsk, GetMessages(ePM_DebugAsk), 0, 6);
        break;
    case MV_PRESSKEY_ENTER:
    case MV_PRESSKEY_SHORTFP:
        switch (Prompt().getCurrentMsgID()) {
        case ePM_DiskFormat:
            Prompt().delMessage(ePM_DiskFormat);
            XmppShell::self().startFormatDisk();
            break;
        case ePM_XmppRegisterAsk:
            Prompt().delMessage(ePM_XmppRegisterAsk);
            XmppShell::self().regist();
            break;
        case ePM_PowerOffAsk:
            Prompt().delMessage(ePM_PowerOffAsk);
            Platform().powerOff();
            break;
        case ePM_ConfResetAsk:
            Prompt().delMessage(ePM_ConfResetAsk);
            SettingLoad(1);
            break;
        case ePM_DebugAsk:
            //Debug here
            Prompt().delMessage(ePM_DebugAsk);
            Network().startThunder();
            break;
        default:
            return false;
        }
        break;
    default:
        ;
    } 
    return true;
}

bool 
NativeHandlerPublic::doNetworkMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->arg1) {
    case MV_NETWORK_CONNECT_OK:
        break;
    case MV_NETWORK_CONNECT_ERROR:
        break;
    case MV_NETWORK_LINK_DOWN:
        break;
    case MV_NETWORK_LINK_UP:
        break;
    case MV_NETWORK_NTPSYNC_AGAIN:
        Network().NTPSync().syncStart();
        break;
    case MV_NETWORK_NTPSYNC_OK:
        H.removeMessages(MessageType_Network, MV_NETWORK_NTPSYNC_AGAIN, 0);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_NTPSYNC_AGAIN, 0, 0), 7200 * 1000); 
        break;
    case MV_NETWORK_NTPSYNC_ERROR:
    case MV_NETWORK_NTPSYNC_TIMEOUT:
        H.removeMessages(MessageType_Network, MV_NETWORK_NTPSYNC_AGAIN, 0);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Network, MV_NETWORK_NTPSYNC_AGAIN, 0, 0), 120 * 1000); 
        break;
    case MV_NETWORK_THUNDER_STATUS:
        break;
    default:
        return false;
    }
    return true;
}

bool 
NativeHandlerPublic::doWeatherMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->arg1) {
    case MV_WEATHER_QUERY:
        WeatherManager::self().query();
        break;
    case MV_WEATHER_QUERY_OK:
        H.removeMessages(MessageType_Weather);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY, 0, 0), 7200 * 1000); 
        break; 
    case MV_WEATHER_QUERY_ERR:
        H.removeMessages(MessageType_Weather);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY, 0, 0), 600 * 1000); 
        break;
    default:
        return false;
    }
    return true;
}

bool 
NativeHandlerPublic::doXmppMessage(Message* msg)
{
    return true;
}

bool 
NativeHandlerPublic::doDatabaseMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    std::string str("");
    if (msg->obj)
        str = (char*)(dynamic_cast<StringData*>(msg->obj)->getData());

    switch (msg->arg1) {
    case MV_DATABASE_RESPONSE:
        XmppShell::self().onResult(msg->arg2, 0, 0, str);
        break;
    case MV_DATABASE_REQUEST:
        FileExplorerThread::getInstance()->sendMessage(msg->arg2, 0, 0, str.c_str());
        break;
    case MV_DATABASE_ADDDISK: 
        if (!str.empty()) {
            std::string json = Disk().getDiskDeviceInfoByDiskName(str, 1);
            if (!json.empty()) {
                JsonObject jo(json);
                std::string isReady = jo.getString("IsMountedAll");
                if (!isReady.compare("1") || msg->arg2 > 5)
                    FileExplorerThread::getInstance()->sendMessage(msg->arg1, 0, 0, json.c_str());
                else 
                    H.sendMessageDelayed(H.obtainMessage(msg->what, msg->arg1, msg->arg2+1, msg->obj), 500);
            }
        }
        break;
    case MV_DATABASE_REMOVEDISK:
        if (!str.empty()) {
            std::string json = Disk().getDiskDeviceInfoByDiskName(str, 1);
            if (!json.empty())
                FileExplorerThread::getInstance()->sendMessage(msg->arg1, 0, 0, json.c_str());
        }
        break;
    default:
        return false;
    }
    return true;
}

bool 
NativeHandlerPublic::doUpgradeMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    int res = 0;
    switch (msg->arg1) {
    case MV_UPGRADE_CHECK:
        if (msg->obj) {
            char* buffer = (char*)(dynamic_cast<StringData*>(msg->obj)->getData());
            res = UpgradeManager::self().check(buffer);
        } else {
            char buffer[128] = {0};
            sysSetting().get("IceBox.UpgradeServer", buffer, 127);
            res = UpgradeManager::self().check(buffer);
        }

        if (IS_CHECKING == res) {
            H.removeMessages(MessageType_Upgrade);
            H.sendMessageDelayed(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_CHECK, 0, msg->obj), 60 * 1000); 
        }
        break;
    case MV_UPGRADE_REQCONFIG:
        if (1 == msg->arg2) {
            //have new version
            if (UpgradeManager::eUT_LOCAL == UpgradeManager::self().type()) {
                //USB Upgrade
                changeState(NativeHandler::kUpgrade);
            } else {
                //Server Upgrade
                char buffer[128] = {0};
                sysSetting().get("IceBox.UpgradeAuto", buffer, 127);
                if (!strncasecmp(buffer, "on", 2)) {
                    //Auto upgrade
                    changeState(NativeHandler::kUpgrade);
                } else {
                    //Wait app agree
                    std::vector<std::string> jids = UserManager::getInstance()->getAllJID();
                    if (jids.size() > 0)
                        XmppShell::self().onUpgradeNotify(jids[0].c_str(), UpgradeManager::self().getNewVersionNumber(), 0);
                    else 
                        XmppShell::self().onUpgradeNotify(0, UpgradeManager::self().getNewVersionNumber(), 0); //TODO test
                }
            }
        } else {
            // error or no new version
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_END, MV_UPGRADE_REQCONFIG, 0)); 
        }
        break;
    case MV_UPGRADE_AGREE:
        changeState(NativeHandler::kUpgrade);
        break;
    case MV_UPGRADE_END:
        H.removeMessages(MessageType_Upgrade);
        H.sendMessageDelayed(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_CHECK, 0, 0), 21600 * 1000); 
        UpgradeManager::self().release();
        break;
    }
    return true; 
}

bool 
NativeHandlerPublic::doPromptMessage(Message* msg)
{
    NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    if (MV_PROMPT_REMOVE == msg->arg1) {
        switch (msg->arg2) {
        case ePM_PowerOffAsk:
        case ePM_DiskFormat:
            break;
        }
        return true;
    } 
    if (MV_PROMTP_TIMEOUT == msg->arg1) {
        switch (msg->arg2) {
        case ePM_PowerOffAsk:
        case ePM_DiskFormat:
            break;
        case ePM_AuthCode:
            Desktop().clearIdentifyCode();
            break;
        }
        return true;
    }
    return false;
}

bool 
NativeHandlerPublic::doDiskMessage(Message* msg)
{
    std::string jsonstr("");
    if (msg->obj) {
        jsonstr = (char*)(dynamic_cast<StringData*>(msg->obj)->getData());
        NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x \n", msg->what, msg->arg1, msg->arg2);
    } else 
        return false;

    switch (msg->arg1) {
    case MV_DISK_ADD:
        NATIVEHANDLER_LOG("add:%s\n", jsonstr.c_str());
        if(isJsonString(jsonstr.c_str())) {
            JsonObject jo(jsonstr);
            std::string bustype = jo.getString("BusType");
            std::string diskname = jo.getString("Disk");
            if(bustype.compare(BUS_TYPE_ATA)) {
                Disk().getDiskCopyQueue()->addUDisk(diskname);
                changeState(NativeHandler::kDiskCopy);
            }
            StringData* data = new StringData(diskname.c_str());
            if (data) {
                H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_ADDDISK, 0, data));
                data->unref();
            }
        }
        break;
    case MV_DISK_REMOVE:
        NATIVEHANDLER_LOG("remove:%s\n", jsonstr.c_str());
        if(isJsonString(jsonstr.c_str())) {
            JsonObject jo(jsonstr);
            std::string diskname = jo.getString("Disk");
            StringData* data = new StringData(diskname.c_str());
            if (data) {
                H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_REMOVEDISK, 0, data));
                data->unref();
            }
        }
        break;
    case MV_DISK_MOUNT:
        NATIVEHANDLER_LOG("mount:%s\n", jsonstr.c_str());
        if (isJsonString(jsonstr.c_str())) {
            JsonObject jo(jsonstr);
            std::string mntdir = jo.getString("MountDir");
            if (!mntdir.empty()) {
                // Do upgrade checking 
                std::string path = mntdir + "/iceupgrade/config.ini";
                if (!access(path.c_str(), F_OK)) {
                    std::string upinifile = "file:///" + path;
                    StringData* data = new StringData(upinifile.c_str());
                    if (data) {
                        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_CHECK, 1, data));
                        data->safeUnref();
                    }
                }
            }
        }
        break;
    case MV_DISK_UNMOUNT:
        NATIVEHANDLER_LOG("unmount:%s\n", jsonstr.c_str());
        break;
    }

    return false;
}

} // namespace IBox
