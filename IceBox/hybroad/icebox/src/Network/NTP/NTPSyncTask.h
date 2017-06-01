#ifndef __NTPSyncTask__H_
#define __NTPSyncTask__H_

#include "Thread.h"

#ifdef __cplusplus

#include <string>

namespace IBox {

class NTPSyncTask {
public:
    friend class _Task;
    NTPSyncTask();
    ~NTPSyncTask();

    void syncStart();

    int setServers(const char* ntp0, const char* ntp1);
    int setTimeout(int t) { mTimeout = t; }

    class _Task : public Thread {
    public :
        enum {
            _eRunning = 1,
            _eFinish = 2,
        };
        _Task(NTPSyncTask& ntp) : _NTPSync(ntp) { };
        ~_Task() { };
        virtual void run();
    private:
        NTPSyncTask& _NTPSync;
    };

private:
    std::string mServer0;
    std::string mServer1;
    int mTimeout;
    int mTaskState; 
};

}
#endif

#endif
