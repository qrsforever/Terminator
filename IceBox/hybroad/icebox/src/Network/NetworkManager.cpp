#include "NetworkAssertions.h"
#include "NetworkManager.h"
#include "EasySetup.h"
#include "ThunderService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "SysSetting.h"
#include "PlatformAPI.h"
#include "IPv4Setting.h"
#include "WiredNetwork.h"
#include "WirelessNetwork.h"

#define WIFI_REMOTE_PORT 9786

extern "C" char *if_indextoname(unsigned ifindex, char *ifname);

namespace IBox {

NetworkManager::NetworkManager() 
    : mEasySetup(0), mLinkSockFd(-1), mWifiRemoteIP("")
    , mThunder(0)
{
    mListenFds[0] = -1;
    mListenFds[1] = -1;

    mNTPSync = new NTPSyncTask();
    mThunder = new ThunderService();

    struct sockaddr_nl addr;
    mLinkSockFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (mLinkSockFd > 0) {
        bzero(&addr, sizeof(addr));
        addr.nl_family = AF_NETLINK;
        addr.nl_groups = RTMGRP_LINK;
        addr.nl_pid = 0;

        bind(mLinkSockFd, (struct sockaddr*)&addr, sizeof(addr));
        fcntl(mLinkSockFd, F_SETFL, O_NONBLOCK);
    }
}

NetworkManager::~NetworkManager()
{
    if (mListenFds[0])
        close(mListenFds[0]);
    if (mListenFds[1])
        close(mListenFds[1]);

    delete mNTPSync;
}

int
NetworkManager::addInterface(NetworkInterface* iface)
{
    if (!iface)
        return -1;
    std::list<NetworkInterface*>::iterator it;
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        if (!strcmp((*it)->ifname(), iface->ifname())) {
            return -1;
        }
    }
    mInterfaces.push_back(iface);
    return 0;
}

NetworkInterface*
NetworkManager::getInterface(const char* ifname)
{
    if (!ifname)
        return 0;
    std::list<NetworkInterface*>::iterator it;
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        if (!strcmp((*it)->ifname(), ifname)) {
            return *it;
        }
    }
    return 0;
}

int
NetworkManager::delInterface(const char* ifname)
{
    if (!ifname)
        return -1;
    std::list<NetworkInterface*>::iterator it;
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        if (!strcmp((*it)->ifname(), ifname)) {
            delete(*it);
            mInterfaces.erase(it);
            break;
        }
    }
    return 0;
}

int 
NetworkManager::setMainInterface(const char* ifname)
{
    if (!ifname)
        return -1;
    bool find = false;
    std::list<NetworkInterface*>::iterator it;
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        (*it)->mIsMain = false;
        if (!strcmp((*it)->ifname(), ifname)) {
            (*it)->mIsMain = true;
            find = true;
        }
    }
    return find ? 0 : -1;
}

NetworkInterface*
NetworkManager::getMainInterface()
{
    std::list<NetworkInterface*>::iterator it;
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        if ((*it)->mIsMain) {
            return *it;
        }
    }
    return 0;
}

int 
NetworkManager::connectWired()
{
    NetworkInterface* iface = getInterface(ETHERNET_IFACE_NAME);
    if (!iface) 
        return -1;

    Thread::start();
    iface->connect();
    setMainInterface(iface->ifname());

    return refresh();
}

int 
NetworkManager::connectWireless()
{
    NetworkInterface* iface = getInterface(WIRELESS_IFACE_NAME);
    if (!iface) 
        return -1;

    Thread::start();
    iface->connect();
    setMainInterface(iface->ifname());

    return refresh();
}

int 
NetworkManager::autoSwitch()
{
    int nettype = 0, status = 0;
    sysSetting().get("NetType", &nettype);

    NetworkInterface* iface = getInterface(ETHERNET_IFACE_NAME);
    if (!iface) 
        return -1;
    NetworkCard::linkUp(ETHERNET_IFACE_NAME);

    status = iface->getPhyDev()->linkStatus(iface->ifname());
    if (NetworkCard::eCT_ETHERNET == nettype 
        && NL_FLG_RUNNING == status) {
        connectWired();
    } else {
        NetworkInterface* iface = getInterface(WIRELESS_IFACE_NAME);
        if (!iface) 
            return -1;

        char easy[8] = { 0 };
        sysSetting().get("Wireless.EasySetup", easy, 8);
        if (!strncasecmp(easy, "1", 1)) {
            NETWORK_LOG("run easysetup\n");
            startEasySetup();
        } else {
            NetworkCard::linkUp(WIRELESS_IFACE_NAME);
            char ssid[128] = { 0 };
            char pass[128] = { 0 };
            sysSetting().get("Wireless.SSID", ssid, 128);
            sysSetting().get("Wireless.Password", pass, 128);
            iface->getPhyDev()->joinAP(ssid, pass);
            NETWORK_LOG("TODO ssid[%s] password[%s]\n", ssid, pass);
        }
    }
    return 0;
}

void 
NetworkManager::startEasySetup()
{
    if (mEasySetup)
        delete mEasySetup;

    NetworkCard::linkDown(WIRELESS_IFACE_NAME);
    NetworkCard::linkUp(WIRELESS_IFACE_NAME);
    mEasySetup = new EasySetup(WIRELESS_IFACE_NAME);
    mEasySetup->start();
}

void 
NetworkManager::closeEasySetup(int saveflg)
{
    if (mEasySetup) {
        if (saveflg) {
            sysSetting().set("Wireless.EasySetup", "0");
            sysSetting().set("Wireless.SSID", mEasySetup->getSsidname().c_str());
            sysSetting().set("Wireless.Password", mEasySetup->getPassword().c_str());
            sysSetting().restore(1);
            mWifiRemoteIP = mEasySetup->getRemoteIP();
        }
        delete mEasySetup;
        mEasySetup = 0;
    }
}

void 
NetworkManager::sendRemotePacket()
{
    if (mWifiRemoteIP.empty())
        return;

    int s = -1;
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    char buffer[16] = { "easysetup:ok" };
    if ((s = socket(AF_INET,SOCK_DGRAM,0)) > 0) {
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(WIFI_REMOTE_PORT);
        addr.sin_addr.s_addr = inet_addr(mWifiRemoteIP.c_str());
        for (int i = 1; i <= 10; ++i) {
            NETWORK_LOG("[%d] WifiRemoteIP = %s\n", i, mWifiRemoteIP.c_str());
            sendto(s, buffer, strlen(buffer) + 1, MSG_DONTWAIT, (struct sockaddr *)&addr, addr_len);
            usleep(200000); //TODO
        }
    }
}

void 
NetworkManager::startThunder()
{
    if (mThunder) {
        NetworkInterface* iface = getMainInterface();
        if (iface) {
            mThunder->etmStop();
            mThunder->etmStart(iface->ifname());
        }
    }
}

void 
NetworkManager::closeThunder()
{
    if (mThunder)
        mThunder->etmStop();
}

ThunderService* 
NetworkManager::getThunderService()
{
    if (!mThunder)
        mThunder = new ThunderService();
    return mThunder;
}

int
NetworkManager::refresh()
{
    int nouse = 1;
    return write(mListenFds[1], &nouse, sizeof(nouse));
}

std::string 
NetworkManager::getAddress()
{
    NetworkInterface* iface = getMainInterface();
    if (iface) {
        if (iface->getAddressType() == NetworkInterface::eAT_IPv4)
            return iface->IPv4Set().getAddress();
        else 
            return iface->IPv6Set().getAddress();
    }
    return "";
}

void 
NetworkManager::run()
{
    fd_set rset;
    int retval = 0;
    int retnum = 0;
    int maxFd = -1;
    int retFd = -1;
    char buffer[8192] = { 0 };
    pipe(mListenFds);
    read(mListenFds[0], &retval, sizeof(retval));
    std::list<NetworkInterface*>::iterator ifaceit;
    for (;;) {
        retFd = -1;
        maxFd = mListenFds[0];
        FD_ZERO(&rset);
        FD_SET(mListenFds[0], &rset);
        FD_SET(mLinkSockFd, &rset);

        for (ifaceit = mInterfaces.begin(); ifaceit != mInterfaces.end(); ++ifaceit) {
            retFd = (*ifaceit)->preSelect(&rset, 0, 0, 0);
            if (retFd > maxFd)
                maxFd = retFd;
        }
        retnum = select(maxFd + 1, &rset, 0, 0, 0);
        if (-1 == retnum) {
            if (EINTR == errno)
                continue;
            return;
        }
        if (FD_ISSET(mListenFds[0], &rset)) {
            read(mListenFds[0], &retval, sizeof(retval));
            if (1 == retnum)
                continue;
        }

        if (FD_ISSET(mLinkSockFd, &rset)) {
            retval = recv(mLinkSockFd, &buffer, 8192, 0);
            do {
                if (retval < 0) {
                    if (errno == EINTR)
                        continue;
                    break;
                }
                for (struct nlmsghdr* nh = (struct nlmsghdr*)buffer; NLMSG_OK(nh, retval); nh = NLMSG_NEXT(nh, retval)) {
                    if (RTM_NEWLINK != nh->nlmsg_type)
                        continue;
                    struct ifinfomsg* msg = (struct ifinfomsg*)NLMSG_DATA(nh);
                    struct rtattr* rta = 0;
                    int rtalen = IFLA_PAYLOAD(nh);
                    if (ARPHRD_LOOPBACK == msg->ifi_type)
                        continue;
                    char devname[32] = { 0 };
                    NetworkInterface* iface = getInterface(if_indextoname(msg->ifi_index, devname));
                    if (iface == getMainInterface()) {
                        iface->getPhyDev()->flagChange(msg->ifi_flags);
                    }
                    /*
                    device = getDevice(if_indextoname(msg->ifi_index, devname)); // include net/if.h
                    if (!device) //|| NetworkCard::NT_WIRELESS == device->getType()) //wifi is complicated now.
                        continue;
                    device->flagChange(msg->ifi_flags); //msg->ifi_change
                    for (rta = IFLA_RTA(msg); RTA_OK(rta, rtalen); rta = RTA_NEXT(rta, rtalen))
                        device->linkChange(rta->rta_type, (char*)RTA_DATA(rta), RTA_PAYLOAD(rta));
                    */
                }
            } while (0 != (retval = recv(mLinkSockFd, &buffer, 8192, 0)));
            if (1 == retnum)
                continue;
        }

        for (ifaceit = mInterfaces.begin(); ifaceit != mInterfaces.end(); ++ifaceit){
            if ((*ifaceit)->postSelect(&rset, 0, 0) > 0 && 1 == retnum)
                break;
        }
    }
}

int 
NetworkManager::init()
{
    char address[128] = { 0 };
    char netmask[128] = { 0 };
    char gateway[128] = { 0 };
    char dns0[128] = { 0 };
    char dns1[128] = { 0 };
    char ntp0[128] = { 0 };
    char ntp1[128] = { 0 };
    int conntype = 0;
    NetworkInterface* iface = 0;

    sysSetting().get("NTPServer0", ntp0, 127);
    sysSetting().get("NTPServer1", ntp1, 127);

    iface = new NetworkInterface(new WiredNetwork(), ETHERNET_IFACE_NAME);
    if (iface) {
        sysSetting().get("Wired.IPv4.Address", address, 127);
        sysSetting().get("Wired.IPv4.Netmask", netmask, 127);
        sysSetting().get("Wired.IPv4.Gateway", gateway, 127);
        sysSetting().get("Wired.IPv4.Dns0", dns0, 127);
        sysSetting().get("Wired.IPv4.Dns1", dns1, 127);
        sysSetting().get("Wired.ConnectType", &conntype);
        iface->IPv4Set().setAddress(address);
        iface->IPv4Set().setNetmask(netmask);
        iface->IPv4Set().setGateway(gateway);
        iface->IPv4Set().setDns0(dns0);
        iface->IPv4Set().setDns1(dns1);
        iface->setProtocolType(conntype);
        iface->setAddressType(NetworkInterface::eAT_IPv4);
        addInterface(iface);
    }

    iface = new NetworkInterface(new WirelessNetwork(), WIRELESS_IFACE_NAME);
    if (iface) {
        sysSetting().get("Wireless.IPv4.Address", address, 127);
        sysSetting().get("Wireless.IPv4.Netmask", netmask, 127);
        sysSetting().get("Wireless.IPv4.Gateway", gateway, 127);
        sysSetting().get("Wireless.IPv4.Dns0", dns0, 127);
        sysSetting().get("Wireless.IPv4.Dns1", dns1, 127);
        sysSetting().get("Wireless.ConnectType", &conntype);
        iface->IPv4Set().setAddress(address);
        iface->IPv4Set().setNetmask(netmask);
        iface->IPv4Set().setGateway(gateway);
        iface->IPv4Set().setDns0(dns0);
        iface->IPv4Set().setDns1(dns1);
        iface->setProtocolType(conntype);
        iface->setAddressType(NetworkInterface::eAT_IPv4);
        addInterface(iface);
    }

    char tz[16] = { 0 };
    sysSetting().get("IceBox.TimeZone", tz, 16);
    if (strlen(tz) > 0) {
        double timezone = strtod(tz, 0);
        if (timezone >= -12.5 && timezone < 12.5)
            Platform().setTZ(timezone);
    }

    NTPSync().setServers(ntp0, ntp1);
    NTPSync().setTimeout(20);
    // Thread::start();
    return 0;
}

NetworkManager& 
NetworkManager::self()
{
    static NetworkManager* sNetworkManager = 0;
    if (!sNetworkManager) {
        sNetworkManager = new NetworkManager();
        sNetworkManager->init();
    }
    return *sNetworkManager;
}

}
