#include "UpgradeAssertions.h"
#include "UpgradeBurning.h"
#include "UpgradeConstant.h"

#include "libzebra.h"
#include <unistd.h>

namespace IBox {

UpgradeBurning::UpgradeBurning(MessageHandler* handler, int taskid)
    : mHandler(handler), mTaskID(taskid), mRunning(false), mInterrupt(false)
{
}

UpgradeBurning::~UpgradeBurning()
{
}

int 
UpgradeBurning::_interrupt() 
{ 
    if (!mRunning)
        return 0;

    mInterrupt = true; 
    for (int i = 10; i > 0; --i) {
        if (!mRunning)
            break;
        usleep(300000);
    }
    if (mRunning) {
        UPGRADE_LOG_ERROR("cannot interrupt!!!\n");
        return -1;
    }
    return 0;
}

int 
UpgradeBurning::cleanup()
{
    if (_interrupt())
        return -1;
    return 0;
}

int  
UpgradeBurning::onError(int err)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_FAIL, err));
    return -1;
}

int 
UpgradeBurning::onProgress(int progress)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_PROGRESS, progress));
    return 0;
}

int
UpgradeBurning::onSuccess(int nouse)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_SUCCESS, nouse));
    return 0;
}

int 
UpgradeBurning::burnSoftware(const char* filepath, bool force)
{
    if (mRunning)
        return onError(-100);

    mSoftwareFile = filepath;
    mForce = force;

    Thread::start();
    return 0;
}

void
UpgradeBurning::run()
{
    mRunning = true;
    {
        if (yhw_upgrade_checkSoftware((char*)mSoftwareFile.c_str(), -1)) {               
            UPGRADE_LOG_ERROR("check upgrade file err.\n");
            onError(-200);
            goto END;
        }

        if (mInterrupt) {
            onError(-500);
            goto END;
        }

        //TODO
        const int msleep = 500;
        const int step = 5;
        const int counts = 100 / step;
        onProgress(step);
        int ret = yhw_upgrade_burnSoftware((char*)mSoftwareFile.c_str(), -1);
        if (ret == 0) { 
            UPGRADE_LOG("upgrade all mtds ok.\n");
            //yhw_upgrade_getBurnProgress(&progress); 
            if (mInterrupt) {
                onError(-500);
                goto END;
            } else {
                for (int i = 1; i < counts; ++i) {
                    onProgress((i+1) * step);
                    usleep(msleep * 1000);
                }
                onSuccess(0);
                goto END;
            }
        }     
        onError(-300);
    }
END:
    mRunning = false;
    mInterrupt = false;
}

}

