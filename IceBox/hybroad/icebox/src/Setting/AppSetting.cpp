#include "Assertions.h"
#include "AppSetting.h"

namespace IBox {

static AppSetting g_appSetting("/root/.hy_app.ini");

AppSetting::AppSetting(std::string fileName)
    : Setting(fileName)
{
}

AppSetting::~AppSetting()
{
}

int
AppSetting::recoverBakSeting()
{
    return 0;
}

AppSetting& appSetting()
{
    return g_appSetting;
}

} // namespace IBox
