#include "Assertions.h"
#include "SettingModuleWeather.h"

#include <stdio.h>

#include "Setting.h"
#include "SysSetting.h"

namespace IBox {

SettingModuleWeather::SettingModuleWeather()
    : SettingModule("WEATHER")
{
}

SettingModuleWeather::~SettingModuleWeather()
{
}

int
SettingModuleWeather::settingModuleRegister()
{
    sysSetting().section(name());

    sysSetting().add("Weather.LastCityID", "101010100");
    sysSetting().add("Weather.GetCityIdUrl", "http://61.4.185.48:81/g/"); 
    // sysSetting().add("Weather.GetWeatherUrl", "http://weatherapi.market.xiaomi.com/wtr-v2/weather?cityId={cityID}");
    sysSetting().add("Weather.GetWeatherUrl", "http://weather.123.duba.net/static/weather_info/{cityID}.html");
    return 0;
}

int SettingWeather()
{
    static SettingModuleWeather s_SettingModuleWeather;
    return 0;
}

} // namespace IBox
