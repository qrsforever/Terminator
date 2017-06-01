#include "Assertions.h"
#include "SettingModule.h"

#include <stdio.h>

namespace IBox {

static SettingModule* g_SettingModuleHead = 0;

SettingModule::SettingModule(const char* name) : mModuleName(name)
{
    mNext = g_SettingModuleHead;
    g_SettingModuleHead = this;
}

SettingModule::~SettingModule()
{
}

void SettingModulesResgister()
{
    SettingModule *p_settingModule = g_SettingModuleHead;
    while (p_settingModule) {
        p_settingModule->settingModuleRegister();
        p_settingModule = p_settingModule->mNext;
    }
}

} // namespace IBox
