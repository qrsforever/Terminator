#include "Assertions.h"
#include "SettingListener.h"

#include <string>
#include <map>
#include <utility>

namespace IBox {

SettingListener::SettingListener()
{
}

SettingListener::~SettingListener()
{
}

int
SettingListener::add(ListenerFunction func)
{
    if (!func)
        return -1;

    mFunctions.push_back(func);
    return 0;
}

int
SettingListener::del(ListenerFunction func)
{
    if (!func)
        return -1;

    std::vector<ListenerFunction>::iterator it;
    for (it = mFunctions.begin(); it != mFunctions.end(); ++it) {
        if ((*it) == func) {
             mFunctions.erase(it);
             break;
        }
    }
    if (it == mFunctions.end())
        return -1;
    return 0;
}

int
SettingListener::call(const char*name, const char* value)
{
    std::vector<ListenerFunction>::iterator it;
    for (it = mFunctions.begin(); it != mFunctions.end(); ++it) {
        (*it)(name, value);
    }
    return 0;
}

} // namespace IBox


/*
//���ڸ���ģ��Լ���setting�ֶμ���ע�ᣬ��׿�汾��������
extern "C" int SettingListenerRegist(const char* name, int type, ListenerFunction func)
{
#ifdef ANDROID
    return 0;
#endif

    if (type) //sysSetting
        Hippo::sysSetting().registListeners(name, func);
    else //AppSetting
        Hippo::appSetting().registListeners(name, func);
    return 0;
}

//���ڸ���ģ��ע��������setting�ֶ�
extern "C" int SettingListenerUnregist(const char* name, int type, ListenerFunction func)
{
#ifdef  ANDROID
    return 0;
#endif

    if (type) //sysSetting
        Hippo::sysSetting().unregistListeners(name, func);
    else //AppSetting
        Hippo::appSetting().unregistListeners(name, func);
    return 0;
}
*/

