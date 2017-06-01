#ifndef _SettingListener_H_
#define _SettingListener_H_

typedef int (*ListenerFunction)(const char*, const char*);

#ifdef __cplusplus

#include <vector>

namespace IBox {

class SettingListener {
public:
    SettingListener();
    ~SettingListener();

    int call(const char* name, const char* value);
    int add(ListenerFunction func);
    int del(ListenerFunction func);
    bool empty() { return mFunctions.empty(); };

private:
    std::vector<ListenerFunction> mFunctions;
};

} 
#endif //__cplusplus

/*
#ifdef __cplusplus
extern "C" {
#endif

int SettingListenerRegist(const char* name, int type, ListenerFunction func);
int SettingListenerUnregist(const char* name, int type, ListenerFunction func);

#ifdef __cplusplus
}
#endif //__cplusplus
*/

#endif //SettingListener_h
