#include "NetworkAssertions.h"
#include "WiredNetwork.h"

#include <linux/ethtool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>


namespace IBox {

WiredNetwork::WiredNetwork() : NetworkCard()
{
}

WiredNetwork::~WiredNetwork()
{
}

int
WiredNetwork::linkStatus(const char* ifname)
{
    struct ifreq ifr;
    struct ethtool_value status;

    int sockfd = -1;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        NETWORK_LOG_ERROR("socket: %s\n", strerror(errno));
        return NL_ERR_UNKNOW;
    }

    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_data = (char*)&status;
    status.cmd = ETHTOOL_GLINK;
    status.data = 0;

    if (-1 == ioctl(sockfd, SIOCETHTOOL, &ifr)) {
        NETWORK_LOG_ERROR("ioctl: %s\n", strerror(errno));
        return NL_ERR_UNKNOW;
    }

    return status.data > 0 ? NL_FLG_RUNNING : NL_FLG_DOWN;
}

int
WiredNetwork::flagChange(int flag)
{
    NETWORK_LOG("%s\n", NetlinkFlagStr(flag));
    if ((flag & IFF_UP) && (flag & IFF_RUNNING)) {
        if (eLS_ETHERNET_UP == mLinkState)
            return 0;
        mLinkState = eLS_ETHERNET_UP;
    } else {
        if (eLS_ETHERNET_DOWN == mLinkState)
            return 0;
        mLinkState = eLS_ETHERNET_DOWN;
    }
    return sendMessage(mLinkState);
}

}
