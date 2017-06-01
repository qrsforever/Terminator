#ifndef __WeatherAssertions__H_
#define __WeatherAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gWeatherModuleLevel;

#define WEATHER_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gWeatherModuleLevel, args)
#define WEATHER_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gWeatherModuleLevel, args)
#define WEATHER_LOG(args...)   LOG(HLG_RUNNING, gWeatherModuleLevel, args)

#endif //__cplusplus

#endif
