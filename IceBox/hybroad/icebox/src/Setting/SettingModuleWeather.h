#ifndef _SettingModuleWeather_H_
#define _SettingModuleWeather_H_

#ifdef __cplusplus

#include "SettingModule.h"

namespace IBox {

class SettingModuleWeather : public SettingModule {
public:
	SettingModuleWeather();
	virtual ~SettingModuleWeather();
	
	virtual int settingModuleRegister();
};

int SettingWeather();

} // namespace IBox
#endif //__cplusplus

#endif // _SettingModuleWeather_H_
