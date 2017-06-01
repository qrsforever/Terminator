#include "NetworkAssertions.h"
#include "NTPSyncTask.h"
#include "NativeHandler.h"
#include "MessageTypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

extern "C" int sntpc_gettime (const char *server, struct timeval *ptv_timeout, struct timespec *pts);

namespace IBox {

NTPSyncTask::NTPSyncTask() 
    : mTaskState(-1)
    , mServer0("")
    , mServer1("")
{
}

NTPSyncTask::~NTPSyncTask()
{
}

int 
NTPSyncTask::setServers(const char* ntp0, const char* ntp1)
{
    if (ntp0)
        mServer0 = ntp0;
    if (ntp1)
        mServer1 = ntp1;
    return 0;
}

void
NTPSyncTask::syncStart()
{
    if (mTaskState == _Task::_eRunning)
        return; 
    (new _Task(*this))->start();
}

void
NTPSyncTask::_Task::run()
{
    _NTPSync.mTaskState = _eRunning;
    {
        NativeHandler& H = defNativeHandler();
        int retval = -1;
        struct timeval tv;
        struct timespec tspec = {0};
        tv.tv_sec = _NTPSync.mTimeout; 
        tv.tv_usec = 0;
        NETWORK_LOG("NTPSever0 = %s\n", _NTPSync.mServer0.c_str());
        retval = sntpc_gettime(_NTPSync.mServer0.c_str(), &tv, &tspec);
        if (retval) {
            tv.tv_sec = _NTPSync.mTimeout; 
            tv.tv_usec = 0;
            NETWORK_LOG("NTPSever1 = %s\n", _NTPSync.mServer1.c_str());
            retval = sntpc_gettime(_NTPSync.mServer1.c_str(), &tv, &tspec);
        }
        if (!retval) {
            tv.tv_sec = tspec.tv_sec;
            tv.tv_usec = tspec.tv_nsec / 1000;
            settimeofday(&tv, 0);
        }

        switch (retval) {
        case 0:
            retval = MV_NETWORK_NTPSYNC_OK;
            break;
        case -1:
            retval = MV_NETWORK_NTPSYNC_ERROR;
            break;
        case -2:
            retval = MV_NETWORK_NTPSYNC_TIMEOUT;
            break;
        default:
            retval = MV_NETWORK_NTPSYNC_ERROR;
        }
        H.sendMessage(H.obtainMessage(MessageType_Network, retval, 0, 0));
    }
    _NTPSync.mTaskState = _eFinish;
    delete this;
}

}
