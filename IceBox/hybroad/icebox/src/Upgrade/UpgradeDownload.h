#ifndef __UpgradeDownload__H_
#define __UpgradeDownload__H_

#include "Thread.h"
#include "CurlEasy.h"
#include "MessageHandler.h"

#include <string>

namespace IBox {

class UpgradeDownload : public Thread , public CurlEasy {
public:
    UpgradeDownload(MessageHandler* handler, int taskid);
    ~UpgradeDownload();

    int wget(const char* url, const char* filepath);
    int cleanup();

    double getTotoalSize() { return mTotalSize; }
    double getNowSize() { return mNowSize; }
    std::string& getOutputFile() { return mOutput; }

protected:
	virtual int receiveData(char *buf, size_t size, size_t nmemb);
    virtual int receiveProgress(double total, double now);
    virtual void run();

    int onError(int err);
    int onProgress(int progres);
    int onSuccess(int nouse);

private:
    int _interrupt();

    FILE* mFp;
    std::string mUrl;
    std::string mOutput;
    bool mRunning;
    bool mInterrupt;
    int mTaskID;

    double mTotalSize;
    double mNowSize;
    int mProgress;

    MessageHandler* mHandler;

};

}

#endif
