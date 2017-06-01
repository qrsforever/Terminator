#include "WeatherAssertions.h"
#include "WeatherRequest.h"
#include "MessageTypes.h"

#include <unistd.h>

namespace IBox {

WeatherRequest::WeatherRequest(MessageHandler* handler, int taskid)
    : mHandler(handler), mTaskID(taskid), mRunning(false), mInterrupt(false)
    , mUrl(""), mData("")
{
    setVerbose(0);
    setTimeout(5);
    setConnectTimeout(3);
}

WeatherRequest::~WeatherRequest()
{
    _interrupt();
}

int 
WeatherRequest::_interrupt()
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
        WEATHER_LOG_ERROR("cannot interrupt!!!\n");
        return -1;
    }
    return 0;
}

int 
WeatherRequest::fetch(const char* url)
{
    if (!url)
        return onError(-100);

    if (mRunning)
        return onError(-200);

    mUrl = url;
    Thread::start();
    return 0;
}

int  
WeatherRequest::onError(int err)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, REQUEST_RESULT_FAIL, err));
    return -1;
}

int
WeatherRequest::onSuccess(int nouse)
{
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(mTaskID, REQUEST_RESULT_SUCCESS, nouse));
    return 0;
}

int 
WeatherRequest::receiveData(char* buf, size_t size, size_t nmemb)
{
    if (mInterrupt)
        return onError(-500);
    mData.append(buf, size * nmemb);
    return size * nmemb;
}

int 
WeatherRequest::receiveProgress(double total, double now)
{
    if (mInterrupt)
        return onError(-500);
    return 0;
}

void 
WeatherRequest::run()
{
    mRunning = true;
    {
        WEATHER_LOG("CurlEasy download: %s\n", mUrl.c_str());
        int res = CurlEasy::download(mUrl.c_str());
        if (0 != res) {
            WEATHER_LOG_ERROR("CurlEasy Download TaskID[%d] Error[%d]!\n", mTaskID, res);
            onError(res);
        } else {
            WEATHER_LOG("CurlEasy Download TaskID[%d] OK!\n", mTaskID);
            onSuccess(0);
        }
    }
    mInterrupt = false;
    mRunning = false;
}

}
