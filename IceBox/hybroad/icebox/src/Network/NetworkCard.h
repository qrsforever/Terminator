#ifndef __NetworkCard_H_
#define __NetworkCard_H_

#ifdef __cplusplus
#include <string>
#include <vector>
#include <list>
#include <map>
#define NL_ERR_UNKNOW -1
#define NL_FLG_DOWN    0
#define NL_FLG_RUNNING 1

namespace IBox {

class NetworkCard {

public:
    ~NetworkCard();

    enum CardType_e {
        eCT_ETHERNET,
        eCT_WIRELESS,
    };
    
    enum LinkStatus_e {
        eLS_UNKNOWN,
        eLS_ETHERNET_UP,
        eLS_ETHERNET_DOWN,
        eLS_WIRELESS_UP,
        eLS_WIRELESS_DOWN,
        eLS_WIRELESS_JOIN_FAIL,
        eLS_WIRELESS_JOIN_SECCESS,
        eLS_WIRELESS_CHECK_SIGNAL,
    };

    virtual int type() = 0;

    static int linkUp(const char* ifname);
    static int linkDown(const char* ifname);
    virtual int linkStatus(const char* ifname) = 0;
    virtual int linkChange(int type, char* data, int size) { return 0; }
    virtual int flagChange(int) = 0;
    virtual int joinAP(const char* ssid, const char* password) { return 0; }

    int sendMessage(int flg);
    const char* getMAC(const char* ifname); //{ return _mMAC.c_str(); }

    int addIfname(const char* ifname);
    int delIfname(const char* ifname);

    NetworkCard();

    static const char* NetlinkFlagStr(unsigned int fl);

protected:
    int mLinkState;

private:
    std::list<std::string> _mIfnames;
    std::string _mDevName;
    std::string _mMAC;
};

}
#endif

#endif
