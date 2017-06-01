#include "XmppAssertions.h"
#include "XmppShell.h"
#include "XmppFormatDisk.h"
#include "XmppLocalPlayer.h"
#include "JsonObject.h"

#include "SysSetting.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "DiskDeviceManager.h"
#include "DiskFormatView.h"
#include "StringData.h"
#include "PlatformAPI.h"
#include "GraphicsLayout.h"
#include "PromptMessage.h"
#include "NetworkManager.h"
#include "ThunderService.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>

extern "C" 
int _XmppCommand(int what, int arg1, int arg2, const char* json)
{
    return IBox::XmppShell::self().onRequest(what, arg1, arg2, json);
}

namespace IBox {

XmppShell::XmppShell()
    : mTryCnt(0), mLocalPlayer(0)
    , mFormatDisk(0)
{
    gloox::createXmppInterface();
    gloox::xmppInterface()->setXmppExecFun(_XmppCommand);
}

XmppShell::~XmppShell()
{
}

int 
XmppShell::onRequest(int what, int arg1, int arg2, const char* json)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x json = %s\n", what, arg1, arg2, (json ? json : "null"));
    if (!json)
        return -1;
    StringData* ojson = 0;
    switch (what) {
    case gloox::XmppInterface::Message_Authentication:
        ojson = new StringData(json);
        if (ojson) {
            sendMessage(obtainMessage(e_HandleAuthentication, what, arg1, ojson)); 
            ojson->safeUnref();
        }
        break;
    case gloox::XmppInterface::Message_Transfer:
    case gloox::XmppInterface::Message_DataBase:
        ojson = new StringData(json);
        if (ojson) {
            NativeHandler& H = defNativeHandler();
            H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_REQUEST, what, ojson));
            ojson->safeUnref();
        }
        break;
    case gloox::XmppInterface::Message_Player:
        ojson = new StringData(json);
        if (ojson) {
            sendMessage(obtainMessage(e_HandlePlayer, arg1, arg2, ojson)); 
            ojson->safeUnref();
        }
        break;
    case gloox::XmppInterface::Message_System:
        ojson = new StringData(json);
        if (ojson) {
            JsonObject obj(json);
            std::string cmd = obj.getString("Command");
            if (!cmd.compare("FormatDisk"))
                sendMessage(obtainMessage(e_HandleFormatDisk, FORMATDISK_PRESTART, 0, ojson)); 
            ojson->safeUnref();
        }
        break;
    default:

        return -1;
    }
    return 0;
}/*}}}*/

int 
XmppShell::onResult(int what, int arg1, int arg2, const char* json)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x json = %s\n", what, arg1, arg2, (json ? json : "null"));
    switch (what) {
    case gloox::XmppInterface::Message_Authentication:
        break;
    case gloox::XmppInterface::Message_Transfer:
        break;
    case gloox::XmppInterface::Message_DataBase:
        break;
    case gloox::XmppInterface::Message_Player:
        break;
    case gloox::XmppInterface::Message_System:
        break;
    default:
        return -1;
    }
    gloox::xmppInterface()->handleServerMessage(json);
    return 0;
}/*}}}*/

int
XmppShell::startFormatDisk()
{/*{{{*/
    sendMessage(obtainMessage(e_HandleFormatDisk, FORMATDISK_START, 0, 0)); 
    return 0;
}/*}}}*/

void 
XmppShell::handleMessage(Message* msg)
{/*{{{*/
    switch (msg->what) {
    case e_HandleAuthentication:
        doAuthenticationMessage(msg);
        break;
    case e_HandlePlayer:
        doLocalPlayerMessage(msg);
        break;
    case e_HandleFormatDisk:
        doFormatDiskMessage(msg);
        break;
    }
}/*}}}*/

int 
XmppShell::login(int flg)
{/*{{{*/
    char username[128] = { 0 };
    char password[128] = { 0 };
    sysSetting().get("IceBox.Username", username, 127);
    sysSetting().get("IceBox.Password", password, 127);

    std::string cmd = flg ? "AddUser" : "Login";
    std::string uid = username; //Platform().getMAC() + std::string("_box@") + std::string("iz25ij1x9vyz");
    std::string pwd = password; //std::string("hybroad_cloudlet@") + Platform().getChipID();
    std::string cer = Platform().getSerial() + std::string("@") + Platform().getChipID();

    JsonObject obj;
    obj.add("Action", "FunctionCall");
    obj.add("FunctionType", "Authentication");
    obj.add("Command", cmd);
    obj.add("JID", uid);
    obj.add("Password", pwd);
    obj.add("Certificate", cer);

    gloox::xmppInterface()->handleOperationMessage(gloox::XmppInterface::Message_Authentication, 0, 0, obj.toString());
    mTryCnt++;
    return 0;
}/*}}}*/

bool 
XmppShell::doAuthenticationMessage(Message* msg)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    if (!msg->obj)
        return false;

    JsonObject obj(dynamic_cast<StringData*>(msg->obj)->getData());
    std::string cmd = obj.getString("Command");
    if (cmd.empty())
        return false;
    NativeHandler& H = defNativeHandler();

    if (!cmd.compare("Login")) { //result of IBox login xmpp server
        std::string res = obj.getString("Response");
        if (!res.compare("Success")) {
            H.sendMessage(H.obtainMessage(MessageType_Xmpp, MV_XMPP_ICEBOX_LOGINOK, 0, 0));
            return true;
        }
        H.sendMessage(H.obtainMessage(MessageType_Xmpp, MV_XMPP_ICEBOX_LOGINERR, 0, 0));
        return true;
    }

    if (!cmd.compare("LoginBox")
        || !cmd.compare("Logout")) {
        H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_REQUEST, msg->arg1, msg->obj));
        return true;
    }

    if (!cmd.compare("AddUser")) {
        std::string pin = obj.getString("Captcha");
        if (!pin.empty()) {
            Desktop().showIdentifyCode(pin.c_str());
            Prompt().addMessageX(ePM_AuthCode, 90);
        }
        return true;
    }

    if (!cmd.compare("RegisterBox")) {
        std::string uname = obj.getString("UserName");
        if (!uname.empty()) {
            Prompt().delMessage(ePM_AuthCode);
            Desktop().clearIdentifyCode();
            Prompt().delMessage(ePM_XmppRegisterUser);
            Prompt().addMessageI(ePM_XmppRegisterUser, (GetMessages(ePM_XmppRegisterUser) + uname).c_str(), 0, 3);  
            H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_REQUEST, msg->arg1, msg->obj));
        }
        return true;
    }

    return false;
}/*}}}*/

bool 
XmppShell::doLocalPlayerMessage(Message* msg)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    if (!msg->obj)
        return false;
    JsonObject obj(dynamic_cast<StringData*>(msg->obj)->getData());
    std::string cmd = obj.getString("Command");
    if (cmd.empty())
        return false;
    std::string jid = obj.getString("JID");
    if (jid.empty())
        return false;
    int playerid = obj.getInt("PlayerID");

    JsonObject response;
	response.add("Action", "FunctionResponse");
	response.add("FunctionType", "Player");
    response.add("Command", cmd);
    response.add("JID", jid);
    switch (XmppLocalPlayer::convert(cmd)) {
    case LOCALPLAYER_CREATE:
        if (mLocalPlayer) {
            std::string& appjid = mLocalPlayer->getAppJid();
            delete mLocalPlayer; 
            mLocalPlayer = 0;
            JsonObject resold;
            resold.add("Action", "FunctionResponse");
            resold.add("FunctionType", "Player");
            resold.add("Command", cmd);
            resold.add("JID", appjid.c_str());
            resold.add("Response", "Failed");
            resold.add("ErrorNo", "-1");
            onResult(gloox::XmppInterface::Message_Player, 0, 0, resold.toString());
        }
        mLocalPlayer = new XmppLocalPlayer(0, jid, this);
        response.add("Response", "Success");
        response.add("PlayerID", mLocalPlayer->getPlayerID());
        response.add("Volume", mLocalPlayer->getVolume());
        response.add("Mute", mLocalPlayer->getMute());
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_CLOSE:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            delete mLocalPlayer; 
            mLocalPlayer = 0;
            if (id == playerid)
                response.add("Response", "Success");
            else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_PLAY:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string url = obj.getString("Parameter");
                if (url.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->play(url); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_PAUSE:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                mLocalPlayer->pause(); 
                response.add("Response", "Success");
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_RESUME:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                mLocalPlayer->resume();
                response.add("Response", "Success");
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_FASTFORWARD:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string speed = obj.getString("Parameter");
                if (speed.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->fastForward(atoi(speed.c_str())); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_FASTREWIND:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string speed = obj.getString("Parameter");
                if (speed.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->fastRewind(atoi(speed.c_str())); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SEEK:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string time = obj.getString("Time");
                if (time.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->seekTo(atoi(time.c_str())); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_GETTOTALTIME:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                response.add("Response", "Success");
                response.add("Time", (int)mLocalPlayer->getTotalTime()); 
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_GETCURRENTTIME:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                response.add("Response", "Success");
                response.add("Time", (int)mLocalPlayer->getCurrentTime());
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SETVOLUME:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string vol = obj.getString("Parameter");
                if (vol.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->setVolume(atoi(vol.c_str())); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SETMUTE:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string mute = obj.getString("Parameter");
                if (mute.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    mLocalPlayer->setMute(atoi(mute.c_str())); 
                    response.add("Response", "Success");
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SWITCHAUDIOCHANNEL:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string channel = mLocalPlayer->switchAudioChannel(); 
                if (channel.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    response.add("Response", "Success");
                    response.add("Status", channel);
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SWITCHAUDIOTRACK:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string track = mLocalPlayer->switchAudioTrack();
                if (track.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    response.add("Response", "Success");
                    response.add("Language", track);
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    case LOCALPLAYER_SWITCHSUBTITLE:
        if (mLocalPlayer) {
            int id = mLocalPlayer->getPlayerID();
            if (id == playerid) {
                std::string subtitle = mLocalPlayer->switchSubtitle();
                if (subtitle.empty()) {
                    response.add("Response", "Failed");
                    response.add("ErrorNo", "-4");
                } else {
                    response.add("Response", "Success");
                    response.add("Language", subtitle);
                }
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "-3");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "-2");
        }
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
        break;
    default:
        response.add("Response", "Failed");
        response.add("ErrorNo", "-100");
        onResult(gloox::XmppInterface::Message_Player, 0, 0, response.toString());
    }
    return true;
}/*}}}*/

bool 
XmppShell::doThunderMessage(Message* msg)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);
    if (!msg->obj)
        return false;
    JsonObject obj(dynamic_cast<StringData*>(msg->obj)->getData());
    std::string cmd = obj.getString("Command");
    if (cmd.empty())
        return false;
    std::string jid = obj.getString("JID");
    if (jid.empty())
        return false;
    JsonObject response;
	response.add("Action", "FunctionResponse");
	response.add("FunctionType", "Thunder");
    response.add("Command", cmd);
    response.add("JID", jid);

    ThunderService* thunder = Network().getThunderService();
    if (!thunder) {
        response.add("Response", "Failed");
        response.add("ErrorNo", "50001");
        onResult(msg->what, 0, 0, response.toString());
        return false;
    }

    if (!cmd.compare("DeviceKey")) {
        if (!thunder->getResult()) {
            std::string key = thunder->getBindKey();
            if (!key.empty()) {
                response.add("Response", "Success");
                response.add("Key", key);
            } else {
                response.add("Response", "Failed");
                response.add("ErrorNo", "50002");
            }
        } else {
            response.add("Response", "Failed");
            response.add("ErrorNo", "50001");
        }
        onResult(msg->what, 0, 0, response.toString());
    }
    return true;
}/*}}}*/

bool 
XmppShell::doFormatDiskMessage(Message* msg)
{/*{{{*/
    XMPP_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    switch (msg->arg1) {
    case FORMATDISK_PRESTART:
        if (msg->obj) {
            JsonObject obj(dynamic_cast<StringData*>(msg->obj)->getData());
            std::string jid = obj.getString("JID");
            if (!mFormatDisk) {
                std::string partition = obj.getString("Disk"); // /dev/sdb1
                std::string type = obj.getString("FilesystemType"); // NTFS
                mFormatDisk = new XmppFormatDisk(jid, partition, type, this);
                Prompt().delMessage(ePM_DiskFormat);
                Prompt().addMessageX(ePM_DiskFormat, 10);
                Desktop().getDiskFormatView()->promptDiskMessage(partition, mFormatDisk->getMountDir(), mFormatDisk->getSize()).inval(0);
            } else {
                JsonObject response;
                response.add("Action", "FunctionResponse");
                response.add("FunctionType", "FormatDisk");
                response.add("JID", jid);
                response.add("Response", "Failed");
                response.add("ErrorCode", "-1");
                onResult(gloox::XmppInterface::Message_System, 0, 0, response.toString());
            }
        }
        break;
    case FORMATDISK_START:
        if (!mFormatDisk) {
            JsonObject response;
            response.add("Action", "FunctionResponse");
            response.add("FunctionType", "FormatDisk");
            response.add("JID", mFormatDisk->getAppJid());
            response.add("Response", "Failed");
            response.add("ErrorCode", "-2");
            onResult(gloox::XmppInterface::Message_System, 0, 0, response.toString());
        } else {
            mFormatDisk->start();
            sendMessage(obtainMessage(e_HandleFormatDisk, FORMATDISK_PROGRESS, 0, 0)); 
        }
        break;
    case FORMATDISK_PROGRESS:
        sendMessageDelayed(obtainMessage(e_HandleFormatDisk, FORMATDISK_PROGRESS, 0, 0), 1000); 
        Desktop().getDiskFormatView()->setProgress(mFormatDisk->getProgress()).inval(0);
        break;
    case FORMATDISK_SUCCESS:
        sendMessageDelayed(obtainMessage(e_HandleFormatDisk, FORMATDISK_FINISH, 0, 0), 3000); 
        Desktop().getDiskFormatView()->promptResultMessage(msg->arg2).inval(0);
        break;
    case FORMATDISK_FAIL:
        sendMessageDelayed(obtainMessage(e_HandleFormatDisk, FORMATDISK_FINISH, 0, 0), 3000); 
        Desktop().getDiskFormatView()->promptResultMessage(msg->arg2).inval(0);
        break;
    case FORMATDISK_FINISH:
        if (mFormatDisk) {
            removeMessages(e_HandleFormatDisk);
            Desktop().clearDiskFormatLayer();
            JsonObject response;
            response.add("Action", "FunctionResponse");
            response.add("FunctionType", "FormatDisk");
            response.add("JID", mFormatDisk->getAppJid());
            if (mFormatDisk->mErrorCode == FORMATDISK_SUCCESS) 
                response.add("Response", "Success");
            else {
                response.add("Response", "Failed");
                response.add("ErrorCode", mFormatDisk->mErrorCode);
            }
            onResult(gloox::XmppInterface::Message_System, 0, 0, response.toString());
            delete mFormatDisk;
            mFormatDisk = 0;
        }
        break;
    }
    return true;
}/*}}}*/

int 
XmppShell::onUpgradeNotify(const char* jid, const char* versionNumber, unsigned int versionSize)
{/*{{{*/
    /*
    if (!jid || !versionNumber)
        return -1;

    char json[1025] = { 0 };
        */
    /*
    snprintf(json, 1024, "{ \"Action\": \"FunctionResponse\", \"FunctionType\": \"SystemNotify\", \"JID\": \"%s\", \"SubCommand\": \"OnlineUpgrade\", \"VersionNumber\": \"%s\", \"VersionSize\": %d }", 
        jid, versionNumber, versionSize);
        */

    //TODO
    //gloox::XmppInterface::Message_SystemNotify
    // return onResult(gloox::XmppInterface::Message_SystemNotify, 0, 0, json);

    NativeHandler& H = defNativeHandler();
    H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_AGREE, 0, 0));
}/*}}}*/

XmppShell& 
XmppShell::self()
{
    static XmppShell* s_XmppShell = 0;
    if (!s_XmppShell)
        s_XmppShell = new XmppShell();
    return *s_XmppShell;
}

}

//XmppShell::self().onRequest(gloox::XmppInterface::Message_FormatDisk, 0, 0, "{ \"Action\": \"FunctionCall\", \"FunctionType\": \"FormatDisk\", \"JID\": \"zpf@iz25b0xvgbmz\", \"Disk\": \"/dev/sdb1\", \"FilesystemType\": \"NTFS\" }");
//XmppShell::self().onRequest(gloox::XmppInterface::Message_SystemNotify, 0, 0, "{ \"Action\": \"FunctionResponse\", \"FunctionType\": \"SystemNotify\", \"JID\": \"zpf@iz25b0xvgbmz\", \"SubCommand\": \"OnlineUpgrade\", \"VersionNumber\": \"1.0.207\" }");
