#include "NetworkAssertions.h"
#include "NetworkCard.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "PlatformAPI.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <linux/if.h>

namespace IBox {

NetworkCard::NetworkCard() : _mMAC(""), mLinkState(eLS_UNKNOWN)
{

}

NetworkCard::~NetworkCard()
{
}

int
NetworkCard::linkUp(const char* ifname)
{
    int ret = 0;
    char cmd[128] = { 0 };
    snprintf(cmd, 127, "ifconfig %s up", ifname);
    return Platform().systemCall(cmd);
}

int
NetworkCard::linkDown(const char* ifname)
{
    int ret = 0;
    char cmd[128] = { 0 };
    snprintf(cmd, 127, "ifconfig %s 0.0.0.0 down", ifname);
    return Platform().systemCall(cmd);
}

const char* 
NetworkCard::getMAC(const char* ifname)
{
    if (!_mMAC.empty())
        return _mMAC.c_str();

    int ret = -1, sockfd = -1;
    char s_macaddr[7] = { 0 };
    char mac[13] = { 0 };
    struct ifreq ifreq;

    memset(&ifreq, 0, sizeof(struct ifreq));
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return 0;
	strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
	if ((ioctl(sockfd, SIOCGIFHWADDR, &ifreq)) < 0)
        goto Err;
	memcpy(s_macaddr,(char*)ifreq.ifr_hwaddr.sa_data, 6);

    snprintf(mac, 12, "%02x%02x%02x%02x%02x%02x",
        s_macaddr[0], s_macaddr[1],
        s_macaddr[2], s_macaddr[3],
        s_macaddr[4], s_macaddr[5]);
    _mMAC = mac;
    close(sockfd);
    return _mMAC.c_str();
Err:
    NETWORK_LOG_ERROR("ifname = %s, error: %s\n", ifname, strerror(errno));
    close(sockfd);
    return 0;
}

int
NetworkCard::addIfname(const char* ifname)
{
    if(!ifname)
        return -1;
    std::list<std::string>::iterator it;
    for(it = _mIfnames.begin(); it != _mIfnames.end(); ++it) {
        if (!(*it).compare(ifname))
            return 0;
    }
    _mIfnames.push_back(ifname);
    return 0; 
}

int 
NetworkCard::delIfname(const char* ifname)
{
    if(!ifname)
        return -1;
    _mIfnames.remove(ifname);
    return 0; 
}

int 
NetworkCard::sendMessage(int flg)
{
    NativeHandler& H = defNativeHandler();

    int arg1 = -1;
    switch (flg) {
    case eLS_ETHERNET_UP:
        arg1 = MV_NETWORK_LINK_UP;
        break;
    case eLS_ETHERNET_DOWN:
        arg1 = MV_NETWORK_LINK_DOWN;
        break;
    case eLS_WIRELESS_UP:
        arg1 = MV_NETWORK_LINK_UP;
        break;
    case eLS_WIRELESS_DOWN:
        arg1 = MV_NETWORK_LINK_DOWN;
        break;
    }
    H.sendMessage(H.obtainMessage(MessageType_Network, arg1, type(), 0));
    return 0;
}

//Debug Use, will delete
const char* 
NetworkCard::NetlinkFlagStr(unsigned int fl)
{
    static char buf[512] = { 0 };
    static struct flag {
        const char *name;
        unsigned int flag;
    } flags[] = {
#define  F(x)   { #x, IFF_##x }
        F(UP),
        F(BROADCAST),
        F(DEBUG),
        F(LOOPBACK),
        F(POINTOPOINT),
        F(NOTRAILERS),
        F(RUNNING),
        F(NOARP),
        F(PROMISC),
        F(ALLMULTI),
        F(MASTER),
        F(SLAVE),
        F(MULTICAST),
#undef F
    };
    char *cp = buf;

    *cp = '\0';

    for (unsigned int i = 0; i < sizeof(flags)/sizeof(*flags); i++) {
        if (fl & flags[i].flag) {
            fl &= ~flags[i].flag;
            cp += sprintf(cp, "%s,", flags[i].name);
        }
    }

    if (fl != 0)
        cp += sprintf(cp, "%x,", fl);

    if (cp != buf)
        cp[-1] = '\0';

    return buf;
}

}
