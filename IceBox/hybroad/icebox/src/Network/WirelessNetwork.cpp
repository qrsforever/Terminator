#include "NetworkAssertions.h"
#include "WirelessNetwork.h"
#include "NativeHandler.h"
#include "MessageTypes.h"

extern "C" {
#include "wireless.h"
}

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

#include <unistd.h>

namespace IBox {

WirelessNetwork::WirelessNetwork(const char* ssid, const char* pass)
   : NetworkCard(), mTaskState(-1)
   , mSSID(ssid), mPassword(pass), mAuthMode(""), mEncrType("")
                                                                                        
{

}

WirelessNetwork::WirelessNetwork()
   : NetworkCard(), mTaskState(-1)
   , mSSID(""), mPassword(""), mAuthMode(""), mEncrType("")
                                                                                        
{
}

int
WirelessNetwork::linkStatus(const char* ifname)
{
    return -1;
}

int
WirelessNetwork::flagChange(int flag)
{
    NETWORK_LOG("%s\n", NetlinkFlagStr(flag));
    if (flag & IFF_UP) {  //flag & IFF_RUNNING not need.
        if (eLS_WIRELESS_UP == mLinkState)
           return 0;
        mLinkState = eLS_WIRELESS_UP;
    } else {
        if (eLS_WIRELESS_DOWN == mLinkState)
          return 0;
        mLinkState = eLS_WIRELESS_DOWN;
    }
    return sendMessage(mLinkState);
}

int
WirelessNetwork::linkChange(int type, char* data, int size)
{
    return -1;
}

void
WirelessNetwork::_Task::run()
{
    _Wifi.mTaskState = _eRunning;
    {
        NativeHandler& H = defNativeHandler();
        int count = 6, signal = 0, ret = 0, ctry = 10, state = MV_NETWORK_JOINAP_ERROR, connected = 0, errcode = -100;
        char ssid[64] = { 0 };
        char auth[64] = { 0 };
        char encr[64] = { 0 };
        yos_net_setWLANType(HYBROAD_WLAN_WPA_SUPPLICANT); 
        yos_net_setWpaSupplicantDriverName((char*)"nl80211"); 
        yos_net_uninitWLAN();
        yos_net_initWLAN();
        yos_net_scanWLAN();
        while (ctry--) { 
            yos_net_getWLANCount(&count); //TODO something is wrong!
            NETWORK_LOG("yos_net_getWLANCount: %d\n", count);
            for (int i = 0; i < count; ++i) {
                yos_net_getWLANResult(i, ssid, &signal, auth, encr);
                NETWORK_LOG("index %d : [%s] %d %s %s vs [%s]\n", i, ssid, signal, auth, encr, _Wifi.mSSID.c_str());
                if (0 == _Wifi.mSSID.compare(ssid)) {
                    ret = yos_net_configWLAN((char*)_Wifi.mSSID.c_str(), (char*)_Wifi.mPassword.c_str(), auth, encr);
                    if (!ret) {
                        ret = yos_net_connectWLAN();
                        if (!ret) {
                            errcode = -400;
                            connected = 0;
                            for (int j = 0; j < ctry; ++j) {
                                yos_net_getWlanConnectStatus(&connected);
                                NETWORK_LOG("GetWlanConnectStatus:%d\n", connected);
                                if (connected) {
                                    NETWORK_LOG("[%s] wifi connect ok!\n", ssid);
                                    _Wifi.mAuthMode = auth;
                                    _Wifi.mEncrType = encr;
                                    state = MV_NETWORK_JOINAP_OK;
                                    break;
                                }
                                sleep(1);
                            }
                        } else  {
                            errcode = -300;
                            NETWORK_LOG_ERROR("yos_net_connectWLAN error.\n");
                        }
                    } else {
                        errcode = -200;
                        NETWORK_LOG_ERROR("yos_net_configWLAN error.\n");
                    }
                    break;
                }
            } 
            if (state == MV_NETWORK_JOINAP_OK)
                break;
            if (0 == ctry % 3)
                yos_net_scanWLAN();
            sleep(1);
        }
        H.sendMessage(H.obtainMessage(MessageType_Network, state, errcode, 0));
    }
    _Wifi.mTaskState = _eFinish;
    delete this;
}

int 
WirelessNetwork::joinAP(const char* ssid, const char* password)
{
    mSSID = ssid;
    mPassword = password;
    if (mTaskState == _Task::_eRunning)
        return 1; 
    (new _Task(*this))->start();
    return 0;
}

}
