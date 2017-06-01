#ifndef _Setting_H_
#define _Setting_H_

#include <map>
#include <vector>
#include <string>

#include "SettingListener.h"

#ifdef __cplusplus

namespace IBox {

class CryptoKey;
class Mutex;
class Setting {
public:
    Setting(std::string fileName);
    virtual ~Setting();

    virtual int load();
    virtual int restore(int flag);
    virtual int empty();
    virtual int recoverBakSeting() { return 0; }

    int section(const char* name);

    int add(const char* name, const char* value, bool encrypt = false, SettingListener* listeners = 0);
    int add(const char* name, const int value, bool encrypt = false, SettingListener* listeners = 0);
    int del(const char* name);

    int set(const char* name, const char* value);
    int get(const char* name, char* value, int valueLen);

    int set(const char* name, const int value);
    int get(const char* name, int* value);

    int registListeners(const char* name, ListenerFunction func);
    int unregistListeners(const char* name, ListenerFunction func);

protected:
    struct SettingItem {
            std::string nName;
            std::string nValue;
            bool nIsEncrypt;
            std::string nEncryptedValue;
            SettingListener* nListeners;
    };

    std::string mFileName;
    std::vector<SettingItem*> mItemArray;
    std::map<std::string, SettingItem*> mItemsMap;
    int dirty;
    Mutex* mMutex;
    CryptoKey* mCryptoKey;
};

void SettingInitAndLoad();
void SettingLoad(int reset = 0);
void SettingSave(int force = 0);

} // namespace IBox

#endif // __cplusplus


#endif // _Setting_H_

