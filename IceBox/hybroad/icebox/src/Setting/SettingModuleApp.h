#ifndef _SettingModuleApp_H_
#define _SettingModuleApp_H_

#ifdef __cplusplus

#include "SettingModule.h"

namespace IBox {

class SettingModuleApp : public SettingModule {
public:
	SettingModuleApp();
	virtual ~SettingModuleApp();
	
	virtual int settingModuleRegister();
};

int SettingApp();

} // namespace IBox
#endif //__cplusplus

#endif // _SettingModuleApp_H_
