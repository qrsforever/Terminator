#include "NetworkAssertions.h"
#include "EasySetup.h"
#include "NetworkCard.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "StringData.h"

extern "C" {
#include "easy_setup.h"
#include "cooee.h"
#include "wireless.h"
}

#include <stdio.h>
#include <unistd.h>

namespace IBox {

EasySetup::EasySetup(const char* ifacename)
    : mSsidname(""), mPassword(""), mRemoteIP("")
    , mIfaceName(ifacename)
{
    yos_net_setWifiMonitor(1);
}

EasySetup::~EasySetup()
{
    easy_setup_stop();
    while (mRunning) {
        NETWORK_LOG_ERROR("is running\n");
        sleep(1);
    }
}

std::string
EasySetup::toString()
{
    NETWORK_LOG("###[get %s : %s from %s]\n", mSsidname.c_str(), mPassword.c_str(),  mRemoteIP.c_str());
    std::string str(mSsidname);
    str.append(" : ");
    str.append(mPassword);
    return str;
}

int 
EasySetup::testResult()
{
    toString();
    //TODO do nothing!
    int count = 6, signal = 0, ret = 0, ctry = 10;
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
        NETWORK_LOG("getWlanCount: %d\n", count);
        for (int i = 0; i < count; ++i) {
            yos_net_getWLANResult(i, ssid, &signal, auth, encr);
            NETWORK_LOG("%d : %s %d %s %s\n", i, ssid, signal, auth, encr);
            if (0 == mSsidname.compare(ssid)) {
                ret = yos_net_configWLAN((char*)mSsidname.c_str(), (char*)mPassword.c_str(), auth, encr);
                if (!ret) {
                    ret = yos_net_connectWLAN();
                    if (!ret) {
                        NETWORK_LOG("EasySetup %s OK!\n", ssid);
                        yos_net_uninitWLAN(); 
                        return 1;
                    }
                }
                break;
            }
        } 
        if (0 == ctry % 3) 
            yos_net_scanWLAN();
        sleep(2);
    }
    yos_net_uninitWLAN(); 
    return 0;
}

void 
EasySetup::run()
{
    mRunning = true;
    {
        int ret = -1, state = MV_NETWORK_EASYSETUP_ERROR;
        int len = 0;
        uint16 val;
        uint16 port;
        char ssid[33];      /* ssid of 32-char length, plus trailing '\0' */
        char password[65];  /* password is 64-char length, plus trailing '\0' */
        char ip[16];        /* ipv4 max length */

        NativeHandler& H = defNativeHandler();
        StringData *data = 0;
        H.sendMessage(H.obtainMessage(MessageType_Network, MV_NETWORK_EASYSETUP_START, 0, 0));

        easy_setup_enable_protocols(1);

        ret = easy_setup_start(mIfaceName.c_str(), 0);
        if (ret) {
            ret = -100;
            goto STOP;
        }

        ret = easy_setup_query(60); //timeout unit:s
        if (-1 == ret) {
            ret = -200;
            goto STOP;
        }

        if (-2 == ret) {
            state = MV_NETWORK_EASYSETUP_TIMEOUT;
            ret = -300;
            goto STOP;
        }
            
        ret = easy_setup_get_ssid(ssid, sizeof(ssid));
        if (0 != ret) {
            ret = -400;
            goto STOP;
        }

        ret = easy_setup_get_password(password, sizeof(password));
        if (0 != ret) {
            ret = -500;
            goto STOP;
        }

        ret = cooee_get_sender_ip(ip, sizeof(ip));
        if (0 != ret) {
            ret = -600;
            goto STOP;
        }

        ret = cooee_get_sender_port(&port);
        if (!ret) {
            NETWORK_LOG_WARNING("no port\n");
        }

        mSsidname = ssid;
        mPassword = password;
        mRemoteIP = ip;
        if (testResult()) {
            char buff[128] = { 0 };
            snprintf(buff, 127, "{\"Essid\":\"%s\", \"Password\":\"%s\"}", mSsidname.c_str(), mPassword.c_str());
            data = new StringData(buff);
            state = MV_NETWORK_EASYSETUP_OK;
        } else 
            ret = -700;
STOP:
        /* must do this! */
        easy_setup_stop();
        yos_net_setWifiMonitor(0);

        H.sendMessage(H.obtainMessage(MessageType_Network, state, ret, data));
        if (data)
            data->safeUnref();
    }
    mRunning = false;
}

}
