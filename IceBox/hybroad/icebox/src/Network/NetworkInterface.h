#ifndef __NetworkInterface__H_
#define __NetworkInterface__H_

#include <sys/select.h>
#include <pthread.h>

#ifdef __cplusplus
#include <string>
#include <map>

#include "IPv4Setting.h"
#include "IPv6Setting.h"

#define DHCP_ROOT_DIR "/var/misc/dhcp"

namespace IBox {

class StateNotify;
class NetworkManager;
class NetworkCard;
class NetworkInterface {
public:
    friend class NetworkManager;
    NetworkInterface(NetworkCard* phydev, const char* ifname = 0);
    ~NetworkInterface();

    const char* ifname() { return mIfaceName.c_str(); }

    enum ProtocolType_e {
        ePT_STATIC,
        ePT_DHCP,
        ePT_PPPOE,
    };

    enum AddressType_e {
        eAT_IPv4,
        eAT_IPv6
    };

    int setProtocolType(int type) { mProtocolType = type; }
    int getProtocolType() { return mProtocolType; }

    int setAddressType(int type) { mAddressType = type; }
    int getAddressType() { return mAddressType; }

    virtual int connect();
    virtual int disconnect();
    virtual int preSelect(fd_set* rset, fd_set* wset, fd_set* eset, struct timeval* timeout);
    virtual int postSelect(fd_set* rset, fd_set* wset, fd_set* eset);

    IPv4Setting& IPv4Set() { return mIPv4Conf; }
    IPv6Setting& IPv6Set() { return mIPv6Conf; }

    NetworkCard* getPhyDev() { return mPhyDev; }

protected:
    std::string mIfaceName; //eg: eth0,rausb0,ppp,eth0.xxxx,rausb0.xxxx
    int mProtocolType;
    int mAddressType;
    IPv4Setting mIPv4Conf;
    IPv6Setting mIPv6Conf;
    NetworkCard* mPhyDev;

    int mReadFD;

    StateNotify* mStateN;
private:
    bool mIsMain;
};

}

#endif

#endif

