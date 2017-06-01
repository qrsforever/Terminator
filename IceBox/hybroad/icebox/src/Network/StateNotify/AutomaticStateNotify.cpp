#include "NetworkAssertions.h"
#include "AutomaticStateNotify.h"
#include "NetworkInterface.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "StringData.h"

#include <string>
#include <stdlib.h>

namespace IBox {

AutomaticSetting::AutomaticSetting(std::string filename) 
    : Setting(filename)
{
    add("interface", "");
    add("reason", "");
    add("pid", "");
    add("errcode", "0");
    add("errdetails", "");
    add("extend_state", "");

    add("ip_address", "0.0.0.0");
    add("subnet_mask", "255.255.255.0");
    add("gateway", "0.0.0.0");

    add("ip6_address", "");
    add("ip6_prefixlen", "");

    add("dns0","0.0.0.0");
    add("dns1", "0.0.0.0");
}

AutomaticSetting::~AutomaticSetting()
{
}

std::string
AutomaticSetting::get(const char* name)
{
    char buffer[128] = { 0 };
    Setting::get(name, buffer, 127);
    return buffer;
}

AutomaticStateNotify::AutomaticStateNotify(NetworkInterface* iface) 
    : StateNotify(iface)
{
}

AutomaticStateNotify::~AutomaticStateNotify()
{
}

int
AutomaticStateNotify::notify(int state) 
{
    NETWORK_LOG("state:%d\n", state);
    if (0 == state)
        return 0;

    std::string rootpath = DHCP_ROOT_DIR ;
    std::string filepath = rootpath + "/dhclient-" + mIface->ifname() + ".info";
    AutomaticSetting* conf = new AutomaticSetting(filepath);
    if (!conf->load()) {
        if (state > 0) {
            //get address success.
            if (NetworkInterface::eAT_IPv4 == mIface->getAddressType()) {
                mIface->IPv4Set().setAddress(conf->get("ip_address").c_str());
                mIface->IPv4Set().setNetmask(conf->get("subnet_mask").c_str());
                mIface->IPv4Set().setGateway(conf->get("gateway").c_str());
                mIface->IPv4Set().setDns0(conf->get("dns0").c_str());
                mIface->IPv4Set().setDns1(conf->get("dns1").c_str());
                NETWORK_LOG("Show: address[%s] netmask[%s] gateway[%s] dns0[%s] dns1[%s]\n", 
                    mIface->IPv4Set().getAddress(), mIface->IPv4Set().getNetmask(),
                    mIface->IPv4Set().getGateway(), mIface->IPv4Set().getDns0(), mIface->IPv4Set().getDns1());

            } else {

            }
            StringData* data = new StringData(mIface->ifname());
            NativeHandler& H = defNativeHandler();
            H.sendMessage(H.obtainMessage(MessageType_Network, MV_NETWORK_CONNECT_OK, 0, data));
            data->safeUnref();
        } else {
            //get address fail.
            int err = atoi(conf->get("errcode").c_str());
            StringData* data = new StringData(mIface->ifname());
            NativeHandler& H = defNativeHandler();
            H.sendMessage(H.obtainMessage(MessageType_Network, MV_NETWORK_CONNECT_ERROR, err, data));
            data->safeUnref();
        }
    }
    delete(conf);
    return 0;
}

}
