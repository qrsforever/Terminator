#ifndef _SettingModule_H_
#define _SettingModule_H_

#ifdef __cplusplus

namespace IBox {

class SettingModule {
public:
	SettingModule(const char* name);
	virtual ~SettingModule();

    virtual int settingModuleRegister() = 0;

	SettingModule*  mNext;

    
    const char* name() { return mModuleName; }
private:
    const char* mModuleName;
};

void SettingModulesResgister();

} // namespace IBox

#endif //__cplusplus

#endif // _SettingModule_H_
