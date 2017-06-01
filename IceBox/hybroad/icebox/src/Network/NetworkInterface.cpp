#include "NetworkAssertions.h"
#include "NetworkInterface.h"
#include "ManualStateNotify.h"
#include "AutomaticStateNotify.h"
#include "NetworkCard.h"
#include "PlatformAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

extern "C" int yos_net_setDNSServer(char *,  char *);

namespace IBox {

NetworkInterface::NetworkInterface(NetworkCard* phydev, const char* ifname) 
    : mPhyDev(phydev), mIfaceName(ifname)
    , mProtocolType(ePT_DHCP), mAddressType(eAT_IPv4)
    , mReadFD(-1), mIsMain(false), mStateN(0)
{
    mPhyDev->addIfname(mIfaceName.c_str());
}

NetworkInterface::~NetworkInterface()
{
    mPhyDev->delIfname(mIfaceName.c_str());
    if (mStateN)
        delete(mStateN);
    mStateN = 0;
    if (mReadFD > 0)
        close(mReadFD);
    mReadFD = -1;
}

int 
NetworkInterface::connect()
{
    NETWORK_LOG("ProtocolType[%d] AddressType[%d]\n", mProtocolType, mAddressType);
    int retcall = 0;
    char commands[128] = { 0 };
    char fifoname[128] = { 0 };

    if (mStateN)
        disconnect();

    switch (mProtocolType) {
    case ePT_STATIC:
        switch (mAddressType) {
        case eAT_IPv4:
             snprintf(commands, 127, "ifconfig %s %s netmask %s; route add default gw %s", mIfaceName.c_str(), 
                 mIPv4Conf.getAddress(), mIPv4Conf.getNetmask(), mIPv4Conf.getGateway()); 
             Platform().systemCall(commands, &retcall);
             yos_net_setDNSServer((char*)mIPv4Conf.getDns0(), (char*)mIPv4Conf.getDns1());
             break;
        case eAT_IPv6:
        default:
            return -1;
        }
        mStateN = new ManualStateNotify(this);

        mStateN->notify(1);
        break;
    case ePT_DHCP:
        if (access(DHCP_ROOT_DIR, F_OK) < 0) {
            snprintf(commands, 127, "mkdir -p %s", DHCP_ROOT_DIR); 
            Platform().systemCall(commands, &retcall);
        }
        snprintf(commands, sizeof(commands), "dhclient.connect AF=%d DBDIR=%s DEBUG=%d IFACE=%s", 
            (eAT_IPv4 == mAddressType ? 4 : 6), DHCP_ROOT_DIR, 0, mIfaceName.c_str());
        snprintf(fifoname, sizeof(fifoname), "%s/dhclient-%s.pipe", DHCP_ROOT_DIR, mIfaceName.c_str());
        if (-1 == mkfifo(fifoname, 00777) && (EEXIST != errno))
            return -1;
        mReadFD = open(fifoname, O_RDONLY | O_NONBLOCK | O_CLOEXEC, 0);
        Platform().systemCall(commands, &retcall);
        NETWORK_LOG("DHCP: %s\n", commands);
        mStateN = new AutomaticStateNotify(this);
        break;
    default:
        return -1;
    }
    return 0;
}

int 
NetworkInterface::disconnect()
{
    NETWORK_LOG("ProtocolType[%d] AddressType[%d]\n", mProtocolType, mAddressType);

    int retcall = 0;
    char commands[128] = { 0 };
    char fifoname[128] = { 0 };

    switch (mProtocolType) {
    case ePT_STATIC:
        switch (mAddressType) {
        case eAT_IPv4:
             snprintf(commands, 127, "ifconfig %s 0.0.0.0 up", mIfaceName.c_str());
             Platform().systemCall(commands, &retcall);
             break;
        case eAT_IPv6:
        default:
            return -1;
        }
        break;
    case ePT_DHCP:
        snprintf(commands, sizeof(commands), "dhclient.disconnect AF=%d DBDIR=%s IFACE=%s", 
            (eAT_IPv4 == mAddressType ? 4 : 6), DHCP_ROOT_DIR, mIfaceName.c_str());
        Platform().systemCall(commands, &retcall);
        break;
    default:
        return -1;
    }
    
    if (mStateN)
        delete(mStateN);
    mStateN = 0;

    if (mReadFD > 0)
        close(mReadFD);
    mReadFD = -1;

    return 0;
}

int 
NetworkInterface::preSelect(fd_set* rset, fd_set* wset, fd_set* eset, struct timeval* timeout)
{
    if (mReadFD > 0 && rset)
        FD_SET(mReadFD, rset);
    return mReadFD;
}

int 
NetworkInterface::postSelect(fd_set* rset, fd_set* wset, fd_set* eset)
{
    int ret = 0;
    int state = -1;

    if (mReadFD > 0 && FD_ISSET(mReadFD, rset)) {
        ret = read(mReadFD, &state, sizeof(state));
        if (0 == ret) {
            NETWORK_LOG_WARNING("read = 0\n");
            char fifoname[128] = { 0 };
            snprintf(fifoname, sizeof(fifoname), "%s/dhclient-%s.pipe", DHCP_ROOT_DIR, mIfaceName.c_str());
            if (-1 == mkfifo(fifoname, 00777) && (EEXIST != errno))
                return -1;
            close (mReadFD);
            mReadFD = open(fifoname, O_RDONLY | O_NONBLOCK | O_CLOEXEC, 0);
        } else if (ret > 0) {
            if (mStateN)
                mStateN->notify(state);
        }
        return 1;
    }
    return 0;
}

}
