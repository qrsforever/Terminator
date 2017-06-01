#ifndef _SettingModuleNetwork_H_
#define _SettingModuleNetwork_H_

#ifdef __cplusplus

#include "SettingModule.h"

namespace IBox {

class SettingModuleNetwork : public SettingModule {
public:
	SettingModuleNetwork();
	virtual ~SettingModuleNetwork();
	
	virtual int settingModuleRegister();
};

int SettingNetwork();

} // namespace IBox
#endif //__cplusplus

#endif // _SettingModuleNetwork_H_

