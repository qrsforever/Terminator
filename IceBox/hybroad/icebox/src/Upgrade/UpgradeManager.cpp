#include "UpgradeAssertions.h"
#include "UpgradeManager.h"
#include "UpgradeConstant.h"
#include "UpgradeConfig.h"
#include "UpgradeDownload.h"
#include "UpgradeBurning.h"

#include "NativeHandler.h"
#include "MessageTypes.h"
#include "PlatformAPI.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

namespace IBox {

UpgradeManager::UpgradeManager() 
    : mRequest(0), mChecking(false), mStarting(false), mBurning(0)
    , mServerUrl(""), mNewVersionNumber(""), mNewSoftwareName("")
{
    mHandler = new UMessageHandler(*this);
}

UpgradeManager::~UpgradeManager()
{
}

int 
UpgradeManager::release()
{
    if (mRequest)
        delete mRequest;
    mRequest = 0;
    if (mBurning)
        delete mBurning;
    mBurning = 0;

    if (!access(TEMP_CONFIG_PATH, F_OK))
        remove(TEMP_CONFIG_PATH);

    if (!access(TEMP_SOFTWARE_PATH, F_OK))
        remove(TEMP_SOFTWARE_PATH);
    return 0;
}

int 
UpgradeManager::check(char* url)
{
    if (!url || strlen(url) < 7)
        return -1;

    if (mChecking)
        return IS_CHECKING;

    std::string req(url);
    char* str = strstr(url, "config.ini");
    if (!str) {
        mServerUrl = req;
        req.append("/config.ini");
    } else {
        mServerUrl = std::string(url, str - url);
        mServerUrl.append("/");
    }

    if (mRequest)
        delete mRequest;
    mRequest = new UpgradeDownload(mHandler, eU_RequestConfig);
    mRequest->wget(req.c_str(), TEMP_CONFIG_PATH);
    mChecking = true;

    return UPGRADE_CHECK;
}

int 
UpgradeManager::type()
{
    if (std::string::npos != mServerUrl.find("file://"))
        return eUT_LOCAL;
    if (std::string::npos != mServerUrl.find("http://"))
        return eUT_SERVER;

    return eUT_NONE;
}

int 
UpgradeManager::start(char* url)
{
    if (mStarting)
        return IS_STARTING;

    std::string req(mServerUrl);
    req.append(mNewSoftwareName);

    if (mRequest)
        delete mRequest;
    mRequest = new UpgradeDownload(mHandler, eU_RequestSoftware);
    mRequest->wget(req.c_str(), TEMP_SOFTWARE_PATH);
    mStarting = true;

    return UPGRADE_START;
}

int 
UpgradeManager::burning(char* url)
{
    if (access(TEMP_SOFTWARE_PATH, R_OK)) {
        UPGRADE_LOG_ERROR("file %s is not found!\n", TEMP_SOFTWARE_PATH);
        return UPGRADE_ERR;
    }

    if (mBurning){
        delete mBurning;
        mBurning = 0;
    }
    mBurning = new UpgradeBurning(mHandler, eU_BurningSoftware);
    mBurning->burnSoftware(TEMP_SOFTWARE_PATH);
    return UPGRADE_BURNING;
}

int 
UpgradeManager::_ParserConfig(std::string& config)
{
    UpgradeConfig parser(config);

    if (!parser.load()) {
        //versionnumber a.b.c
        mNewVersionNumber = parser.getVersionNumber();
        mNewSoftwareName  = parser.getSoftwareName();
        UPGRADE_LOG("[%s vs %s] software: %s\n", Platform().getBuildVersionString(), mNewVersionNumber.c_str(), mNewSoftwareName.c_str());

        int versionNumber = 0;
        const char* pver = strrchr(mNewVersionNumber.c_str(), '.');
        if (pver)
            versionNumber = atoi(pver+1);
        else 
            versionNumber = atoi(mNewVersionNumber.c_str());
        
        if (versionNumber > Platform().getBuildVersionNumber()) {
            //Have new version
            return HAVE_NEW_VERSION;
        }
    }
    return NO_NEW_VERSION;
}

int 
UpgradeManager::_VerifySoftware(std::string& software)
{
    //TODO
    return SOFTWARE_VERIFY_OK;
}

int 
UpgradeManager::onRequestConfig(int result, int value)
{
    UPGRADE_LOG("result:%d value:%d\n", result, value);

    NativeHandler& H = defNativeHandler();
    switch (result) {
    case RESULT_FAIL:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQCONFIG, -1, 0)); 
        mRequest->cleanup();
        mChecking = false;
        break;
    case RESULT_SUCCESS:
        if (HAVE_NEW_VERSION == _ParserConfig(mRequest->getOutputFile()))
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQCONFIG, 1, 0)); 
        else
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQCONFIG, 0, 0)); 
        mRequest->cleanup();
        mChecking = false;
        break;
    case RESULT_PROGRESS:
        break;
    }
    return 0;
}

int 
UpgradeManager::onRequestSoftware(int result, int value)
{
    UPGRADE_LOG("result:%d value:%d\n", result, value);

    NativeHandler& H = defNativeHandler();
    switch (result) {
    case RESULT_FAIL:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQSOFTWARE, -1, 0)); 
        mRequest->cleanup();
        mStarting = false;
        break;
    case RESULT_SUCCESS:
        if (SOFTWARE_VERIFY_OK == _VerifySoftware(mRequest->getOutputFile()))
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQSOFTWARE, 1, 0)); 
        else 
            H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_REQSOFTWARE, -2, 0)); 
        mStarting = false;
        break;
    case RESULT_PROGRESS:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_DPROGRESS, value, 0)); 
        break;
    }
    return 0;
}

int 
UpgradeManager::onBurningSoftware(int result, int value)
{
    UPGRADE_LOG("result:%d value:%d\n", result, value);

    NativeHandler& H = defNativeHandler();
    switch (result) {
    case RESULT_FAIL:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_BURNING, value, 0)); 
        mRequest->cleanup();
        break;
    case RESULT_SUCCESS:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_BURNING, 0, 0)); 
        mRequest->cleanup();
        break;
    case RESULT_PROGRESS:
        H.sendMessage(H.obtainMessage(MessageType_Upgrade, MV_UPGRADE_BPROGRESS, value, 0)); 
        break;
    }
    return 0;
}

UpgradeManager& 
UpgradeManager::self()
{
    static UpgradeManager* s_um = 0;
    if (!s_um)
        s_um = new UpgradeManager();
    return *s_um;
}

void 
UpgradeManager::UMessageHandler::handleMessage(Message* msg)
{
    UPGRADE_LOG("msg:%d %d %d\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->what) {
    case eU_RequestConfig:
        mUM.onRequestConfig(msg->arg1, msg->arg2);
        break;
    case eU_RequestSoftware:
        mUM.onRequestSoftware(msg->arg1, msg->arg2);
        break;
    case eU_BurningSoftware:
        mUM.onBurningSoftware(msg->arg1, msg->arg2);
        break;
    default:
        ;
    }
    return;
}

}
