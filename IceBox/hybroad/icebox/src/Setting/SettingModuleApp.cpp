#include "Assertions.h"
#include "SettingModuleApp.h"

#include <stdio.h>

#include "Setting.h"
#include "SysSetting.h"

namespace IBox {

SettingModuleApp::SettingModuleApp()
    : SettingModule("APP")
{
}

SettingModuleApp::~SettingModuleApp()
{
}

int
SettingModuleApp::settingModuleRegister()
{
    sysSetting().section(name());

    sysSetting().add("IceBox.Username", "000767010211_box@iz25ij1x9vyz");
    sysSetting().add("IceBox.Password", "hybroad_cloudlet@12345678901234567890123456789012");
    sysSetting().add("IceBox.UpgradeServer", "http://182.92.77.172/iceupgrade/");
    sysSetting().add("IceBox.UpgradeAuto", "on");
    sysSetting().add("IceBox.TimeZone", "8.0");
    return 0;
}

int SettingApp()
{
    static SettingModuleApp s_SettingModuleApp;
    return 0;
}

} // namespace IBox
