#ifndef ___WEATHERMANAGER__H_
#define ___WEATHERMANAGER__H_ 

#include "MessageHandler.h"
#include <string>

namespace IBox {

class WeatherRequest;
class WeatherParser;
class WeatherManager {
public:
    friend class InnerHandler;

    static WeatherManager& self();

    ~WeatherManager();

    int query();
    int cleanup();

    int getImgNO() { return mImgNO; }
    int getAQI() { return mAQI; }
    int getNowT() { return mNowT; }
    int getMinT() { return mMinT; }
    int getMaxT() { return mMaxT; }

private:
    WeatherManager();

    int _queryCityID(const char* url);
    int _queryWeather(const char* url);
    int _parserData(const char* url);

    int onRequestCityID(int, int);
    int onRequestWeather(int, int);

    class InnerHandler : public MessageHandler {
    public:
        InnerHandler(WeatherManager& wm) : mWM(wm) { }
        ~InnerHandler() { }
    protected:
        void handleMessage(Message* msg);
    private:
        WeatherManager& mWM;
    };

private:
    InnerHandler* mHandler;
    WeatherRequest* mRequest;
    WeatherParser* mParser;
    std::string mCityIdUrl;
    std::string mWeatherUrl;
    std::string mLastCityId;

    int mImgNO;
    int mAQI;
    int mNowT;
    int mMaxT;
    int mMinT;
};

inline WeatherManager& Weather() 
{
    return WeatherManager::self();
}

}

#endif
