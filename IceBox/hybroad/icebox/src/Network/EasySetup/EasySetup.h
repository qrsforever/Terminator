#ifndef __EASYSETUP__H_ 
#define __EASYSETUP__H_

#include "Thread.h"
#include <string>

namespace IBox {

class EasySetup : public Thread {
public:
    EasySetup(const char*);
    ~EasySetup();

    std::string& getSsidname() { return mSsidname; }
    std::string& getPassword() { return mPassword; }
    std::string& getRemoteIP() { return mRemoteIP; }

    std::string toString();

    int testResult();

protected:
    virtual void run();

private:
    bool mRunning;
    std::string mIfaceName;
    std::string mSsidname;
    std::string mPassword;
    std::string mRemoteIP;
};

}
#endif
