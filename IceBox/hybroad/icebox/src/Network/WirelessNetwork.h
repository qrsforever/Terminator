#ifndef __WirelessNetwork_H_
#define __WirelessNetwork_H_

#include "NetworkCard.h"
#include "Thread.h"

#ifdef __cplusplus
#include <string>

namespace IBox {

class WirelessNetwork : public NetworkCard {
public:
    WirelessNetwork(const char* ssid, const char* pass);
    WirelessNetwork();
    ~WirelessNetwork();

    int type() { return eCT_WIRELESS; }

    virtual int linkStatus(const char* ifname);
    virtual int linkChange(int type, char* data, int size);
    virtual int flagChange(int flag);
    virtual int joinAP(const char* ssid, const char* password); 

    class _Task : public Thread {
    public :
        enum {
            _eRunning = 1,
            _eFinish = 2,
        };
        _Task(WirelessNetwork& wifi) : _Wifi(wifi) { };
        ~_Task() { };
        virtual void run();
    private:
        WirelessNetwork& _Wifi;
    };

private:
    std::string mSSID;
    std::string mPassword;
    std::string mAuthMode;
    std::string mEncrType;
    int mTaskState; 
};

}

#endif

#endif
