#include "WeatherAssertions.h"
#include "LogModule.h"

static int s_WeatherModuleFlag = 0;
int gWeatherModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule WeatherModule("weather", s_WeatherModuleFlag, gWeatherModuleLevel);

} // namespace Hippo

