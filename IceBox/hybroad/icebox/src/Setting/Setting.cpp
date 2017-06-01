#include "Assertions.h"
#include "Setting.h"
#include "AppSetting.h"
#include "SysSetting.h"
#include "SettingModule.h"
#include "SettingModuleApp.h"
#include "SettingModuleNetwork.h"
#include "SettingModuleWeather.h"

#include "CryptoKey.h"
#include "Mutex.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

namespace IBox {

Setting::Setting(std::string fileName)
    : mFileName(fileName)
{
    mMutex = new Mutex();
    mCryptoKey = new CryptoKey();
}

Setting::~Setting()
{
    empty();
    delete(mMutex);
    delete(mCryptoKey);
}

int
Setting::load()
{
    FILE *fp = NULL;
    std::string tag(""), value(""), line("");
    std::string::size_type position, posspace;
    char ch;
    char valueRead[1024] = {0}; //the largest length ?

    // checkFileDigest(mFileName.c_str());
    if (( fp = fopen(mFileName.c_str(), "r"))== NULL)
        return -1;

    //line eg. ip = 110.1.1.252
    while ((ch = fgetc(fp)) != (char)EOF || !line.empty()) { //incase of the last line without 0A end
        if(ch != '\n' && ch != (char)EOF) {
            line += ch;
            continue;
        }

        position = line.find('=');
        if (position == std::string::npos) {
            SECURE_LOG_WARNING("check content[%s] is item!\n", line.c_str());
            line.clear();
            continue;
        }
        tag = line.substr(0, position);
        posspace = tag.find(' ');
        if (posspace != std::string::npos)
            tag = tag.substr(0, posspace);
        value = line.substr(position+1);
        posspace = value.find_first_not_of(' ');
        if (posspace != std::string::npos)
            value = value.substr(posspace);

        if (!get(tag.c_str(), valueRead, 1024)) {
            //SECURE_LOG("##set [%s] to map\n", tag.c_str());
            std::map<std::string, SettingItem*>::iterator it = mItemsMap.find(tag.c_str());
            if (it != mItemsMap.end()) {
                if (it->second->nIsEncrypt) {
                    mCryptoKey->decrypt(valueRead, value.c_str(), value.size());
                    it->second->nEncryptedValue = value.c_str();
                    set(tag.c_str(), valueRead);
                } else
                    set(tag.c_str(), value.c_str());
            }
        } else {
            //SECURE_LOG("##add [%s] to map\n", tag.c_str());
            add(tag.c_str(), value.c_str());
        }
        line.clear();
    }

    fclose(fp);
    return 0;
}

int
Setting::restore(int flag)
{
    FILE *fp = NULL;
    std::string line("");
    std::vector<SettingItem*>::iterator it;

    if(!dirty && !flag) {
        SECURE_LOG_WARNING("%s no changed! dirty = %d, flag = %d no need to save.\n",
                mFileName.c_str(), dirty, flag);
        return -1;
    }

    if (( fp = fopen(mFileName.c_str(), "wb"))== NULL)
        return -1;

    mMutex->lock();
    for (it = mItemArray.begin(); it != mItemArray.end(); ++it) {
        SettingItem* item = *it;
        if (item->nName.find('[') != std::string::npos && item->nName.rfind(']') != std::string::npos)
            line = "\n" + item->nName +'\n';
        else {
            if (item->nIsEncrypt)
                line = item->nName + '=' + item->nEncryptedValue + '\n';
            else
                line = item->nName + '=' + item->nValue + '\n';
        }

        uint len = fwrite(line.c_str(), 1, line.length(), fp);
        if(len != line.length()) {
            SECURE_LOG_ERROR("write %s failed!\n", mFileName.c_str());
            mMutex->unlock();
            return -1;
        }
        line.clear();
    }
    mMutex->unlock();

    fclose(fp);
    dirty = 0;
    // updateFileDigest(mFileName.c_str());
    return 0;
}

int
Setting::empty()
{
    std::vector<SettingItem*>::iterator it;

    mMutex->lock();
    for (it = mItemArray.begin(); it != mItemArray.end(); ++it)
        delete (*it);
    mItemArray.clear();
    mItemsMap.clear();
    mMutex->unlock();

    return 0;
}

int 
Setting::section(const char* name)
{
    char buffer[128] = { 0 };
    snprintf(buffer, 127, "[ %s ]", name);
    return add(buffer, "");
}

int
Setting::add(const char* name, const char* value, bool encrypt, SettingListener* listeners)
{
    SettingItem* item = new SettingItem();
    if (!item)
        return -1;
    item->nName = name;
    item->nIsEncrypt = encrypt;
    item->nListeners = listeners;
    if (encrypt) {
        char plain[512] = {0};
        mCryptoKey->decrypt(plain, value, strlen(value));
        item->nValue = plain;
        item->nEncryptedValue = value;
    } else
        item->nValue = value;

    mMutex->lock();
    mItemArray.push_back(item);
    mItemsMap.insert(std::make_pair(item->nName, item));
    mMutex->unlock();
    return 0;
}

int
Setting::add(const char* name, const int value, bool encrypt, SettingListener* listeners)
{
    char buffer[32];
    sprintf(buffer, "%d", value);
    return add(name, buffer, encrypt, listeners);
}

int
Setting::del(const char* name)
{
    std::vector<SettingItem*>::iterator it;

    mMutex->lock();
    for (it = mItemArray.begin(); it != mItemArray.end(); ++it) {
        SettingItem* item = *it;
        if (name == item->nName) {
            mItemsMap.erase(item->nName);
            mItemArray.erase(it);
            delete item;
            break;
        }
    }
    mMutex->unlock();
    return 0;
}

int
Setting::set(const char* name, const char* value)
{
    mMutex->lock();
    std::map<std::string, SettingItem*>::iterator it = mItemsMap.find(name);
    if (it != mItemsMap.end()) {
        if (it->second->nIsEncrypt) {
            char encrypted[1024] = {0};
            mCryptoKey->encrypt(encrypted, value, strlen(value));
            it->second->nEncryptedValue = encrypted;
        }

        std::string *stringValue = &(it->second->nValue);
        if ((*stringValue).compare(value)) {
            *stringValue = value;
            dirty = 1;
            if (it->second->nListeners)
                it->second->nListeners->call(name, value);
        }
        mMutex->unlock();
        return 0;
    }
    mMutex->unlock();
    return -1;
}

int
Setting::get(const char* name, char* value, int valueLen)
{
    mMutex->lock();
    std::map<std::string, SettingItem*>::iterator it = mItemsMap.find(name);
    if (it != mItemsMap.end()) {
        std::string *stringValue = &(it->second->nValue);
        int stringLen = stringValue->size();
        if (stringLen < valueLen) {
            strcpy(value, stringValue->c_str());
            mMutex->unlock();
            return 0;
        }
    }
    mMutex->unlock();
    return -1;
}

int
Setting::set(const char* name, const int value)
{
    char buffer[32];
    sprintf(buffer, "%d", value);
    return set(name, buffer);
}

int
Setting::get(const char* name, int* value)
{
    char buffer[32] = { 0 };
    get(name, buffer, 31);
    *value = atoi(buffer);
    return 0;
}

int
Setting::registListeners(const char* name, ListenerFunction func)
{
    std::map<std::string, SettingItem*>::iterator it = mItemsMap.find(name);
    if (it != mItemsMap.end()) {
        if (it->second->nListeners)
            it->second->nListeners->add(func);
        else {
            it->second->nListeners = new SettingListener();
            it->second->nListeners->add(func);
       }
    } else {
        SECURE_LOG_WARNING("The %s is not settingItem\n",name);
    }
    return 0;
}

int
Setting::unregistListeners(const char* name, ListenerFunction func)
{
    std::map<std::string, SettingItem*>::iterator it = mItemsMap.find(name);
    if (it != mItemsMap.end()) {
        if (it->second->nListeners) {
            it->second->nListeners->del(func);
            if (it->second->nListeners->empty())
                it->second->nListeners = 0;
        } else {
            SECURE_LOG_WARNING("The setting %s no listen functions\n", name);
        }
    } else
       SECURE_LOG_WARNING("The setting %s is not exist\n", name);
    return 0;
}

void SettingInitAndLoad()
{
    SettingApp();
    SettingNetwork();
    SettingWeather();

    SettingModulesResgister();

    SettingLoad();
}

void SettingLoad(int reset)
{
    if (reset) {
        sysSetting().empty();
        appSetting().empty();
        SettingModulesResgister();
        SettingSave(1);
    } else {
        if (sysSetting().load() < 0)
            SettingLoad(1);
        if (appSetting().load() < 0)
            SettingLoad(1);
    }
}

void SettingSave(int force)
{
    sysSetting().restore(force);
    appSetting().restore(force);
}

} // namespace Hippo
