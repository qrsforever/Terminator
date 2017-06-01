#ifndef __XmppShell__H_
#define __XmppShell__H_

#include "MessageHandler.h"
#include "XmppInterface.h"
#include <string>

#define LOGIN_MAX_TRYCOUNT 3

namespace IBox {

class XmppFormatDisk;
class XmppLocalPlayer;
class XmppShell : public MessageHandler {
public:
    ~XmppShell();
    static XmppShell& self();
    int onRequest(int what, int arg1, int arg2, std::string json) { return onRequest(what, arg1, arg2, json.c_str()); }
    int onRequest(int what, int arg1, int arg2, const char* json);

    int onResult(int what, int arg1, int arg2, std::string json) { return onResult(what, arg1, arg2, json.c_str()); }
    int onResult(int what, int arg1, int arg2, const char* json);
    int onUpgradeNotify(const char* jid, const char* versionNumber, unsigned int versionSize);

    int startFormatDisk();

    enum {
        e_HandleAuthentication,
        e_HandlePlayer,
        e_HandleThunder,
        e_HandleFormatDisk = 100,
    };

    int login(int flg = 0);
    int regist() { mTryCnt = 0 ; return login(1); }

    int mTryCnt;
protected:
    void handleMessage(Message* msg);
    bool doAuthenticationMessage(Message* msg);
    bool doLocalPlayerMessage(Message* msg);
    bool doThunderMessage(Message* msg);
    bool doFormatDiskMessage(Message* msg);

private:
    XmppShell();
    XmppFormatDisk* mFormatDisk;
    XmppLocalPlayer* mLocalPlayer;
};

}

#endif
