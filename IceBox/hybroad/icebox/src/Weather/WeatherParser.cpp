#include "WeatherAssertions.h"
#include "WeatherParser.h"
#include "JsonObject.h"

#include <string.h>
#include <stdlib.h>

namespace IBox {

WeatherParser::WeatherParser(const char* data)
    : mText(data)
    , mImgNO(0), mAQI(0), mNowT(0), mMaxT(0), mMinT(0)

{

}

WeatherParser::~WeatherParser()
{
    
}

int 
WeatherParser::exec(int type)
{
    
    switch (type) {
    case USE_DUBA_API:
        {
            int len = strlen("weather_callback(");
            if (mText.size() < len)
                return -1;
            return _doDuBa(mText.substr(len, mText.size() - len - 1));
        }
    case USE_XIAOMI_API:
        return _doXiaoMi(mText);
    default:
        ;
    }
    return -1;
}

//http://weather.123.duba.net/static/weather_info/101010100.html
int 
WeatherParser::_doDuBa(std::string json)
{
    if (!isJsonString(json.c_str()))
        return -1;

    WEATHER_LOG("\njson:%s\n", json.c_str());
    std::string tmpstr("");
    JsonObject info(json);

    tmpstr = info.getString("weatherinfo");
    if (!isJsonString(tmpstr.c_str()))
        return -1;

    JsonObject obj(tmpstr);

    tmpstr = obj.getString("pm");
    if (!tmpstr.empty())
        mAQI = atoi(tmpstr.c_str());

    tmpstr = obj.getString("temp");
    if (!tmpstr.empty())
        mNowT = atoi(tmpstr.c_str());

    tmpstr = obj.getString("temp1");
    if (!tmpstr.empty())
        _splitTemp(tmpstr);

    tmpstr = obj.getString("img1");
    if (!tmpstr.empty())
        mImgNO = atoi(tmpstr.c_str());
    else {
        tmpstr = obj.getString("img_title1");
        if (!tmpstr.empty())
            mImgNO = _switchImgNO(tmpstr);
    }

    _show();
    return 0;
}

//http://weatherapi.market.xiaomi.com/wtr-v2/weather?cityId=101010100
int 
WeatherParser::_doXiaoMi(std::string json)
{
    if (!isJsonString(json.c_str()))
        return -1;

    WEATHER_LOG("\njson:%s\n", json.c_str());
    std::string tmpstr("");
    JsonObject info(json);

    tmpstr = info.getString("realtime");
    if (!isJsonString(tmpstr.c_str()))
        return -1;

    JsonObject realtime(tmpstr);

    tmpstr = realtime.getString("temp");
    if (!tmpstr.empty())
        mNowT = atoi(tmpstr.c_str());

    tmpstr = info.getString("aqi");
    if (!isJsonString(tmpstr.c_str()))
        return -1;
    
    JsonObject api(tmpstr);

    tmpstr = api.getString("aqi");
    if (!tmpstr.empty())
        mAQI = atoi(tmpstr.c_str());

    tmpstr = info.getString("forecast");
    if (!isJsonString(tmpstr.c_str()))
        return -1;

    JsonObject forecast(tmpstr);

    tmpstr = forecast.getString("temp1");
    if (!tmpstr.empty())
        _splitTemp(tmpstr);

    tmpstr = forecast.getString("img1");
    if (!tmpstr.empty())
        mImgNO = atoi(tmpstr.c_str());
    else {
        tmpstr = forecast.getString("img_title1");
        if (!tmpstr.empty())
            mImgNO = _switchImgNO(tmpstr);
    }

    _show();
    return 0;
}

int 
WeatherParser::_splitTemp(std::string& temp)
{
    const char* str = temp.c_str();
    int t1 = 0, t2 = 0;
    t1 = atoi(str); 

    str = strchr(str, '~');
    if (!str)
        t2 = mNowT;
    else 
        t2 = atoi(str + 1);
    if (t1 > t2) {
        mMaxT = t1; 
        mMinT = t2;
    } else {
        mMaxT = t2; 
        mMinT = t1;
    }
    return 0;
}

int 
WeatherParser::_switchImgNO(std::string& title)
{
    const char* pstr = title.c_str(); 
    if (!pstr)
        return ImgNO_UnDef;
        
    if (!strcmp(pstr, "晴"))
        return ImgNO_0;
    if (!strcmp(pstr, "多云"))
        return ImgNO_1;
    if (!strcmp(pstr, "阴"))
        return ImgNO_2;
    if (!strcmp(pstr, "阵雨"))
        return ImgNO_3;
    if (!strcmp(pstr, "雷阵雨"))
        return ImgNO_4;
    if (!strcmp(pstr, "雷阵雨并伴有冰雹"))
        return ImgNO_5;
    if (!strcmp(pstr, "雨夹雪"))
        return ImgNO_6;
    if (!strcmp(pstr, "小雨"))
        return ImgNO_7;
    if (!strcmp(pstr, "中雨"))
        return ImgNO_8;
    if (!strcmp(pstr, "大雨"))
        return ImgNO_9;
    if (!strcmp(pstr, "暴雨"))
        return ImgNO_10;
    if (!strcmp(pstr, "大暴雨"))
        return ImgNO_11;
    if (!strcmp(pstr, "特大暴雨"))
        return ImgNO_12;
    if (!strcmp(pstr, "阵雪"))
        return ImgNO_13;
    if (!strcmp(pstr, "小雪"))
        return ImgNO_14;
    if (!strcmp(pstr, "中雪"))
        return ImgNO_15;
    if (!strcmp(pstr, "大雪"))
        return ImgNO_16;
    if (!strcmp(pstr, "暴雪"))
        return ImgNO_17;
    if (!strcmp(pstr, "雾"))
        return ImgNO_18;
    if (!strcmp(pstr, "冻雨"))
        return ImgNO_19;
    if (!strcmp(pstr, "沙尘暴"))
        return ImgNO_20;
    if (!strcmp(pstr, "小雨转中雨"))
        return ImgNO_21;
    if (!strcmp(pstr, "中雨转大雨"))
        return ImgNO_22;
    if (!strcmp(pstr, "大雨转暴雨"))
        return ImgNO_23;
    if (!strcmp(pstr, "暴雨转大暴雨"))
        return ImgNO_24;
    if (!strcmp(pstr, "大暴雨转特大暴雨"))
        return ImgNO_25;
    if (!strcmp(pstr, "小雪转中雪"))
        return ImgNO_26;
    if (!strcmp(pstr, "中雪转大雪"))
        return ImgNO_27;
    if (!strcmp(pstr, "大雪转暴雪"))
        return ImgNO_28;
    if (!strcmp(pstr, "浮尘"))
        return ImgNO_29;
    if (!strcmp(pstr, "扬沙"))
        return ImgNO_30;
    if (!strcmp(pstr, "强沙尘暴"))         
        return ImgNO_31;

    return ImgNO_UnDef;
}

void 
WeatherParser::_show()
{
    WEATHER_LOG("ImgNO[%d] AQI[%d] Temp[%d  %d ~ %d]\n", mImgNO, mAQI, mNowT, mMinT, mMaxT);
}

}
