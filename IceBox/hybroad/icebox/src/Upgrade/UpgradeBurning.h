#ifndef __UpgradeBurning__H_
#define __UpgradeBurning__H_

#include "Thread.h"
#include "MessageHandler.h"
#include <string>

namespace IBox {

class UpgradeBurning : public Thread {
public:
    UpgradeBurning(MessageHandler* handler, int taskid);
    ~UpgradeBurning();

    int cleanup();

    int burnSoftware(const char* filepath, bool force = false);
    
protected:
    virtual int onError(int err);
    virtual int onProgress(int progres);
    virtual int onSuccess(int nouse);

    virtual void run();

private:
    int _interrupt();

    MessageHandler* mHandler;
    int mTaskID;
    bool mRunning;
    bool mInterrupt;
    bool mForce;
    std::string mSoftwareFile;
};

}

#endif
