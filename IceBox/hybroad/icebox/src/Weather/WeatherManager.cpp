#include "WeatherAssertions.h"
#include "WeatherManager.h"
#include "WeatherRequest.h"
#include "WeatherParser.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "SysSetting.h"

namespace IBox {

WeatherManager::WeatherManager()
    : mHandler(0), mRequest(0), mParser(0)
    , mImgNO(0), mAQI(0), mNowT(0), mMaxT(0), mMinT(0)
{
    mHandler = new InnerHandler(*this);
}

WeatherManager::~WeatherManager()
{

}

int 
WeatherManager::cleanup()
{
    if (mRequest)
        delete mRequest;
    mRequest = 0;

    if (mParser)
        delete mParser;
    mParser = 0;

    return 0;
}

int 
WeatherManager::query()
{
    char buffer[256] = { 0 };
    sysSetting().get("Weather.GetCityIdUrl", buffer, 255); 
    mCityIdUrl = buffer;
    sysSetting().get("Weather.GetWeatherUrl", buffer, 255);
    mWeatherUrl = buffer;
    sysSetting().get("Weather.LastCityID", buffer, 255);
    mLastCityId = buffer;
    return _queryCityID(mCityIdUrl.c_str());
}

int 
WeatherManager::_queryCityID(const char* url)
{
    if (mRequest)
        delete mRequest;
    mRequest = new WeatherRequest(mHandler, eW_RequestCityID);
    return mRequest->fetch(url);
}

int 
WeatherManager::_queryWeather(const char* url)
{
    if (mRequest)
        delete mRequest;
    mRequest = new WeatherRequest(mHandler, eW_RequestWeather);
    return mRequest->fetch(url);
}

int 
WeatherManager::_parserData(const char* data)
{
    int type = -1;

    if (std::string::npos != mWeatherUrl.find("weatherapi.market.xiaomi"))
        type = USE_XIAOMI_API;
    else if (std::string::npos != mWeatherUrl.find("weather.123.duba.net"))
        type = USE_DUBA_API;

    WEATHER_LOG("parser type : %d\n", type);
    WeatherParser parser(data);
    if (!parser.exec(type)) {
        mImgNO = parser.getImgNO();
        mAQI = parser.getAQI();
        mNowT = parser.getNowT();
        mMaxT = parser.getMaxT();
        mMinT = parser.getMinT();
        return 0;
    }
    return -1;
}

int 
WeatherManager::onRequestCityID(int result, int value)
{
    WEATHER_LOG("result[%d] value[%d]\n", result, value);
    NativeHandler& H = defNativeHandler();
    switch (result) {
    case REQUEST_RESULT_FAIL:
        if (!mLastCityId.empty()) {
            std::string url(mWeatherUrl);
            std::string::size_type pos = url.find("{cityID}"); 
            url.replace(pos, 8, mLastCityId.c_str());
            return _queryWeather(url.c_str()); 
        }
        H.sendMessage(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY_ERR, -110, 0));
        cleanup();
        break;
    case REQUEST_RESULT_SUCCESS:
        //Request: http://61.4.185.48:81/g/
        //Respose: var ip="124.207.21.140";var id=101010100;if(typeof(id_callback)!="undefined"){id_callback();}
        if (mRequest) {
            std::string cityId("");
            std::string::size_type pos = -1;
            std::string url(mWeatherUrl);
            std::string data = mRequest->getData();
            pos = data.find("id=");
            if (pos > 0)
                cityId = data.substr(pos + 3, 9);
            pos = url.find("{cityID}"); 
            if (pos > 0)
                url.replace(pos, 8, cityId.c_str());
            if (!cityId.empty()) {
                sysSetting().set("Weather.LastCityID", cityId.c_str());
                return _queryWeather(url.c_str()); 
            }
        }
        H.sendMessage(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY_ERR, -210, 0));
        cleanup();
        break;
    }
    return 0;
}

int 
WeatherManager::onRequestWeather(int result, int value)
{
    WEATHER_LOG("result[%d] value[%d]\n", result, value);
    NativeHandler& H = defNativeHandler();
    switch (result) {
    case REQUEST_RESULT_FAIL:
        H.sendMessage(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY_ERR, -320, 0));
        cleanup();
        break;
    case REQUEST_RESULT_SUCCESS:
        if (mRequest) {
            if (!_parserData(mRequest->getData().c_str()))
                H.sendMessage(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY_OK, 0, 0));
        }
        H.sendMessage(H.obtainMessage(MessageType_Weather, MV_WEATHER_QUERY_ERR, -420, 0));
        cleanup();
        break;
    }
    return 0;
}

WeatherManager& 
WeatherManager::self()
{
    static WeatherManager* s_w = 0;
    if (!s_w)
        s_w = new WeatherManager();
    return *s_w;
}

void 
WeatherManager::InnerHandler::handleMessage(Message* msg)
{
    WEATHER_LOG("msg:%d %d %d\n", msg->what, msg->arg1, msg->arg2);
    switch (msg->what) {
    case eW_RequestCityID:
        mWM.onRequestCityID(msg->arg1, msg->arg2);
        break;
    case eW_RequestWeather:
        mWM.onRequestWeather(msg->arg1, msg->arg2);
        break;
    default:
        ;
    }
    return;
}

}
