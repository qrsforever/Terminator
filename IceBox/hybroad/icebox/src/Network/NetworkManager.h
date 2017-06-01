#ifndef __NetworkManager__H_
#define __NetworkManager__H_

#include "Thread.h"
#include "NTPSyncTask.h"
#include "NetworkInterface.h"

#define ETHERNET_IFACE_NAME "gphy"
#define WIRELESS_IFACE_NAME "wlan0"

#ifdef __cplusplus

#include <list>

namespace IBox {

class ThunderService;
class EasySetup;
class NetworkInterface;
class NetworkManager : public Thread {
public:
    static NetworkManager& self();

    int init();
    int refresh();

    int addInterface(NetworkInterface* iface);
    NetworkInterface* getInterface(const char* ifname);
    int delInterface(const char* ifname);

    virtual void run();

    int setMainInterface(const char* ifname);
    NetworkInterface* getMainInterface();

    int connectWired();
    int connectWireless();
    int autoSwitch();

    NTPSyncTask& NTPSync() { return *mNTPSync; }

    void startEasySetup();
    void closeEasySetup(int saveflg);
    void sendRemotePacket();

    void startThunder();
    void closeThunder();
    ThunderService* getThunderService();

    std::string getAddress();

private:
    NetworkManager();
    ~NetworkManager();

    int mListenFds[2];
    int mLinkSockFd;

    std::string mWifiRemoteIP;

    std::list<NetworkInterface*> mInterfaces;
    
    NTPSyncTask *mNTPSync;
    EasySetup* mEasySetup;
    ThunderService* mThunder;
};

inline NetworkManager& Network() { return NetworkManager::self(); }

}
#endif

#endif
