#include "Assertions.h"
#include "SettingModuleNetwork.h"

#include <stdio.h>

#include "Setting.h"
#include "SysSetting.h"

namespace IBox {

SettingModuleNetwork::SettingModuleNetwork()
    : SettingModule("NETWORK")
{
}

SettingModuleNetwork::~SettingModuleNetwork()
{
}

int
SettingModuleNetwork::settingModuleRegister()
{
    sysSetting().section(name());

    //TODO Test
    sysSetting().add("NetType", 0);

    sysSetting().add("NTPServer0", "s1a.time.edu.cn");
    sysSetting().add("NTPServer1", "s1b.time.edu.cn");

    sysSetting().add("Wired.ConnectType", 1);
    sysSetting().add("Wired.IPv4.Address", "10.0.0.146");
    sysSetting().add("Wired.IPv4.Netmask", "255.255.255.0");
    sysSetting().add("Wired.IPv4.Gateway", "10.0.0.253");
    sysSetting().add("Wired.IPv4.Dns0", "10.0.0.253");
    sysSetting().add("Wired.IPv4.Dns1", "4.2.2.2");

    sysSetting().add("Wireless.EasySetup", "1");
    sysSetting().add("Wireless.SSID", "");
    sysSetting().add("Wireless.Password", "");
    sysSetting().add("Wireless.ConnectType", 1);
    sysSetting().add("Wireless.IPv4.Address", "10.0.0.146");
    sysSetting().add("Wireless.IPv4.Netmask", "255.255.255.0");
    sysSetting().add("Wireless.IPv4.Gateway", "10.0.0.253");
    sysSetting().add("Wireless.IPv4.Dns0", "10.0.0.253");
    sysSetting().add("Wireless.IPv4.Dns1", "4.2.2.2");

    return 0;
}

int SettingNetwork()
{
    static SettingModuleNetwork s_SettingModuleNetwork;
    return 0;
}

} // namespace IBox
