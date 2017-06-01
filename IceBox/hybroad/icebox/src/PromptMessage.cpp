#include "PromptMessage.h"
#include "PlatformAPI.h"

#include <string>
#include <map>

namespace IBox { 

static std::map<int, std::string> gPromptMessages;
static int gInited = 0;

void _InitMessage()
{
    //PowserOff
    gPromptMessages.insert(std::make_pair(ePM_SoftwareVersion, "版本号: " + std::string(Platform().getBuildVersionString())));
    gPromptMessages.insert(std::make_pair(ePM_PowerOffAsk, "是否确认关机?"));
    gPromptMessages.insert(std::make_pair(ePM_RebootAsk, "是否确认重启?"));
    gPromptMessages.insert(std::make_pair(ePM_ConfResetAsk, "是否确认恢复配置?"));
    gPromptMessages.insert(std::make_pair(ePM_DebugAsk, "是否确认开发调试?"));

    //Network
    gPromptMessages.insert(std::make_pair(ePM_NetworkError, "网络连接失败!"));
    gPromptMessages.insert(std::make_pair(ePM_NtpSyncError, "时间同步失败!"));

    //XMPP
    gPromptMessages.insert(std::make_pair(ePM_XmppRegisterAsk, "是否确认注册用户?"));
    gPromptMessages.insert(std::make_pair(ePM_BoxNotConnected, "丹顶云盘还没连接上!"));
    gPromptMessages.insert(std::make_pair(ePM_BoxLoginSuccess, "丹顶云盘登录成功!"));
    gPromptMessages.insert(std::make_pair(ePM_BoxLoginFail, "丹顶云盘登录失败!"));
    gPromptMessages.insert(std::make_pair(ePM_XmppRegisterUser, "注册用户名:"));

    //Upgrade
    gPromptMessages.insert(std::make_pair(ePM_UDownloadSuccess, "升级下载成功."));
    gPromptMessages.insert(std::make_pair(ePM_UDownloadFail, "升级下载失败."));
    gPromptMessages.insert(std::make_pair(ePM_UVerifyFail, "升级校验失败."));
    gPromptMessages.insert(std::make_pair(ePM_UBurningFail, "升级烧写失败."));
}

const char* GetMessages(int msgid)
{
    if (!gInited) {
        _InitMessage();
        gInited = 1;
    }
     
    std::map<int, std::string>::iterator it = gPromptMessages.find(msgid);
    if (it != gPromptMessages.end())
        return it->second.c_str();

    return 0;
}

}
