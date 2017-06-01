#ifndef __WeatherRequest__H_
#define __WeatherRequest__H_

#include "Thread.h"
#include "CurlEasy.h"
#include "MessageHandler.h"

#define REQUEST_RESULT_FAIL      -1
#define REQUEST_RESULT_SUCCESS    0

#include <string>

enum {
    eW_RequestCityID = 0,
    eW_RequestWeather = 1,
};

namespace IBox {

class WeatherRequest : public Thread, public CurlEasy {
public:
    WeatherRequest(MessageHandler* handler, int taskid);
    ~WeatherRequest();

    std::string& getUrl() { return mUrl; }
    std::string& getData() { return mData; }

    int fetch(const char* url);

protected:
	virtual int receiveData(char *buf, size_t size, size_t nmemb);
    virtual int receiveProgress(double total, double now);
    virtual void run();

    int onError(int err);
    int onSuccess(int nouse);

private:
    int _interrupt();

    MessageHandler* mHandler;
    int mTaskID;
    bool mRunning;
    bool mInterrupt;
    std::string mUrl;
    std::string mData;
};

}

#endif
