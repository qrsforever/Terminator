#ifndef __UpgradeManager__H_
#define __UpgradeManager__H_ 

#include "MessageHandler.h"
#include <string>

namespace IBox {

class UpgradeDownload;
class UpgradeBurning;
class UpgradeManager {
public:
    friend class UMessageHandler;
    static UpgradeManager& self();
    ~UpgradeManager();

    int check(char* url);
    int start(char* url = 0);
    int burning(char* url = 0);
    int release();

    enum {
        eUT_NONE,
        eUT_LOCAL, 
        eUT_SERVER,
    };
    int type();

    const char* getNewVersionNumber() { return mNewVersionNumber.c_str(); }

private:
    int onRequestConfig(int, int);
    int onRequestSoftware(int, int);
    int onBurningSoftware(int, int);

    int _ParserConfig(std::string& config);
    int _VerifySoftware(std::string& software);

    class UMessageHandler : public MessageHandler {
    public:
        UMessageHandler(UpgradeManager& um) : mUM(um) { }
        ~UMessageHandler() { }
    protected:
        void handleMessage(Message* msg);
    private:
        UpgradeManager& mUM;
    };
private:
    UpgradeManager();

    std::string mServerUrl;
    std::string mNewVersionNumber;
    std::string mNewSoftwareName;

    bool mChecking;
    bool mStarting;

    UMessageHandler* mHandler;
    UpgradeDownload* mRequest;
    UpgradeBurning*  mBurning;
};

}

#endif
