#include "UpgradeAssertions.h"
#include "UpgradeDownload.h"
#include "UpgradeConstant.h"

#include <unistd.h>

namespace IBox {

UpgradeDownload::UpgradeDownload(MessageHandler* handler, int taskid)
    : mHandler(handler), mTaskID(taskid), mRunning(false), mInterrupt(false)
    , mUrl(""), mOutput(""),  mFp(0)
{

}

UpgradeDownload::~UpgradeDownload()
{
    _interrupt();
}

int 
UpgradeDownload::wget(const char* url, const char* filepath)
{
    UPGRADE_LOG("url = [%s] filepath = [%s]\n", url, filepath);
    if (mRunning)
        return onError(-100);

    mUrl = url;
    mOutput = filepath;

    Thread::start();
    return 0;
}

int 
UpgradeDownload::_interrupt() 
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

    if (mFp) {
        fclose(mFp);
        mFp = 0;
    }
    return 0;
}

int 
UpgradeDownload::cleanup()
{
    if (_interrupt())
        return -1;

    if (!access(mOutput.c_str(), R_OK))
        remove(mOutput.c_str());
    mUrl = "";
    return 0;
}

int  
UpgradeDownload::onError(int err)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_FAIL, err));

    if (mFp) {
        fclose(mFp);
        mFp = 0;
    }
    return -1;
}

int 
UpgradeDownload::onProgress(int progress)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_PROGRESS, progress));
    return 0;
}

int
UpgradeDownload::onSuccess(int nouse)
{
    if (mFp) {
        fclose(mFp);
        mFp = 0;
    }
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, RESULT_SUCCESS, nouse));
    return 0;
}

int 
UpgradeDownload::receiveData(char* buf, size_t size, size_t nmemb)
{
    if (mInterrupt)
        return onError(-500);

    if (!mFp && NULL == (mFp = fopen(mOutput.c_str(), "w")))
        return onError(-300);

    if (fwrite(buf, size, nmemb, mFp) == EOF)
        return onError(-400);

    return size * nmemb;
}

int 
UpgradeDownload::receiveProgress(double total, double now)
{
    if (mInterrupt)
        return onError(-500);

    if (!mTotalSize && !total)
        mTotalSize = total;

    int progress = (int)((now * 100)/total);
    if (progress == mProgress)
        return 0;

    mProgress = progress;
    mNowSize  = now;
    return onProgress(progress);
}

void 
UpgradeDownload::run()
{
    mRunning = true;
    {
        mProgress = 0;
        mTotalSize = 0.0;
        mNowSize = 0.0;

        int res = CurlEasy::download(mUrl.c_str());
        if (0 != res) {
            UPGRADE_LOG_ERROR("CurlEasy Download Error[%d]!\n", res);
            onError(res);
        } else {
            UPGRADE_LOG("CurlEasy Download OK!\n");
            if (100 == mProgress) 
                onSuccess(0);
        }
    }
    mRunning = false;
    mInterrupt = false;
}

}
