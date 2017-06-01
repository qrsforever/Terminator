#ifndef __ThunerService__H_
#define __ThunerService__H_

#include "Thread.h"
#include "CurlEasy.h"

#include <string>

#define THUNDER_RUN_ERROR  -1
#define THUNDER_KEY_BIND    1

namespace IBox {

class MessageHandler;
class ThunderService : public CurlEasy {
public:
    ThunderService(MessageHandler* handler = 0);
    ~ThunderService();

    int etmStart(std::string ifname);
    int etmStop();
    bool isRunning() { return mTaskState == _Task::_eRunning; }

    int checkStatus();

    class _Task : public Thread {
    public :
        enum {
            _eRunning = 1,
            _eFinish = 2,
        };
        _Task(ThunderService& thunder) : _Thunder(thunder) { }
        ~_Task() { };
        virtual void run();
    private:
        ThunderService& _Thunder;
    };

    std::string getBindKey() { return mBindKey; }
    int getResult() { return mResult; }

protected:
	virtual int receiveData(char *buf, size_t size, size_t nmemb);
    virtual int receiveProgress(double total, double now);

private:
    MessageHandler* mHandler;
    std::string mBindKey;
    int mResult;
    bool mEtmStart;
    int mTaskState;

};

}

#endif
