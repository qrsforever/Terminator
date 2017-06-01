#ifndef __WeatherParser__H_
#define __WeatherParser__H_

#define USE_DUBA_API    1
#define USE_XIAOMI_API  2

#include <string>

namespace IBox {

enum ImgNO_ {
    ImgNO_0 = 0,    // 00：晴
    ImgNO_1,        // 01：多云
    ImgNO_2,        // 02：阴
    ImgNO_3,        // 03：阵雨
    ImgNO_4,        // 04：雷阵雨
    ImgNO_5,        // 05：雷阵雨并伴有冰雹
    ImgNO_6,        // 06：雨夹雪
    ImgNO_7,        // 07：小雨
    ImgNO_8,        // 08：中雨
    ImgNO_9,        // 09：大雨
    ImgNO_10 = 10,  // 10：暴雨
    ImgNO_11,       // 11：大暴雨
    ImgNO_12,       // 12：特大暴雨
    ImgNO_13,       // 13：阵雪  
    ImgNO_14,       // 14：小雪
    ImgNO_15,       // 15：中雪
    ImgNO_16,       // 16：大雪
    ImgNO_17,       // 17：暴雪
    ImgNO_18,       // 18：雾
    ImgNO_19,       // 19：冻雨
    ImgNO_20 = 20,  // 20：沙尘暴
    ImgNO_21,       // 21：小雨转中雨
    ImgNO_22,       // 22：中雨转大雨
    ImgNO_23,       // 23：大雨转暴雨
    ImgNO_24,       // 24：暴雨转大暴雨
    ImgNO_25,       // 25：大暴雨转特大暴雨
    ImgNO_26,       // 26：小雪转中雪
    ImgNO_27,       // 27：中雪转大雪
    ImgNO_28,       // 28：大雪转暴雪
    ImgNO_29,       // 29：浮尘
    ImgNO_30 = 30,  // 30：扬沙
    ImgNO_31,       // 31：强沙尘暴           
    ImgNO_UnDef
};

class WeatherParser {
public:
    WeatherParser(const char* data);
    ~WeatherParser();

    int exec(int type);

    int getImgNO() { return mImgNO; }
    int getAQI() { return mAQI; }
    int getNowT() { return mNowT; }
    int getMinT() { return mMinT; }
    int getMaxT() { return mMaxT; }

private:
    int _doDuBa(std::string json);
    int _doXiaoMi(std::string json);

    int _splitTemp(std::string&);
    int _switchImgNO(std::string&);

    void _show();

private:
    std::string mText;
    int mImgNO;
    int mAQI;
    int mNowT;
    int mMaxT;
    int mMinT;
};

}

#endif

#if 0

http://weather.123.duba.net/static/weather_info/101010100.html
weather_callback(
{
    "weatherinfo": 
    {
        "city_en":"beijing",
        "city":"北京",
        "cityid":"101010100",
        "week":"星期四",
        "date_y":"2015年06月18日",
        "fchh":4,
        "date":"201506181800",
        "dt":4,
        "fl1":"微风",
        "fl2":"微风",
        "fl3":"微风",
        "fl4":"微风",
        "fl5":"微风",
        "fl6":"微风",
        "fx1":"无持续风向",
        "fx2":"无持续风向",
        "img1":"4",
        "img2":"4",
        "img3":"4",
        "img4":"1",
        "img5":"0",
        "img6":"0",
        "img7":"0",
        "img8":"1",
        "img9":"1",
        "img10":"1",
        "img11":"2",
        "img12":"0",
        "img_single":"4",
        "img_title1":"雷阵雨",
        "img_title2":"雷阵雨",
        "img_title3":"雷阵雨",
        "img_title4":"多云",
        "img_title5":"晴",
        "img_title6":"晴",
        "img_title7":"晴",
        "img_title8":"多云",
        "img_title9":"多云",
        "img_title10":"多云",
        "img_title11":"阴",
        "img_title12":"晴",
        "img_title_single":"雷阵雨",
        "pm":"33",
        "pm-level":"优",
        "pm-num":"1",
        "pm-pubtime":"2015-06-18 16:00:00",
        "pm-url":"",
        "rl1":"19:45",
        "rc1":"04:45",
        "rl2":"19:45",
        "rc2":"04:45",
        "sd":"18%",
        "st1":"4",
        "st2":"4",
        "st3":"0",
        "st4":"0",
        "st5":"1",
        "st6":"2",
        "temp":"31",
        "temp1":"20℃",
        "tempF1":"87.8℃~68℃",
        "temp2":"26℃~17℃",
        "tempF2":"78.8℃~62.6℃",
        "temp3":"32℃~19℃",
        "tempF3":"89.6℃~66.2℃",
        "temp4":"32℃~21℃",
        "tempF4":"89.6℃~69.8℃",
        "temp5":"32℃~22℃",
        "tempF5":"89.6℃~71.6℃",
        "temp6":"32℃~22℃",
        "tempF6":"89.6℃~71.6℃",
        "wd":"西南风",
        "weather1":"雷阵雨",
        "weather2":"雷阵雨",
        "weather3":"晴",
        "weather4":"晴",
        "weather5":"多云",
        "weather6":"阴",
        "wind1":"微风",
        "wind2":"微风",
        "wind3":"微风",
        "wind4":"微风",
        "wind5":"微风",
        "wind6":"微风",
        "ws":"2级",
        "wse":"2"
     },
    "update_time":"2015-06-18 18:25:01","t":1434623344,"from":"cache"
})


http://weatherapi.market.xiaomi.com/wtr-v2/weather?cityId=101010100
{
    "forecast":
        {"city":"北京","city_en":"beijing","cityid":"101010100","date":"","date_y":"2015年04月13日","fchh":"11","fl1":"3-4级转小于3级","fl2":"小于3级","fl3":"4-5级转5-6级","fl4":"4-5级转小于3级","fl5":"小于3级","fl6":"微风","fx1":"北风转无持续风向","fx2":"微风","img1":"","img10":"","img11":"","img12":"","img2":"","img3":"","img4":"","img5":"","img6":"","img7":"","img8":"","img9":"","img_single":"","img_title1":"晴","img_title10":"多云","img_title11":"晴","img_title12":"晴","img_title2":"晴","img_title3":"晴","img_title4":"多云","img_title5":"阴","img_title6":"多云","img_title7":"晴","img_title8":"晴","img_title9":"晴","img_title_single":"","index":"冷","index48":"","index48_d":"","index48_uv":"","index_ag":"极易发","index_cl":"较适宜","index_co":"舒适","index_d":"","index_ls":"适宜","index_tr":"适宜","index_uv":"中等","index_xc":"较适宜","st1":"","st2":"","st3":"","st4":"","st5":"","st6":"","temp1":"18℃~5℃","temp2":"24℃~10℃","temp3":"25℃~13℃","temp4":"19℃~6℃","temp5":"22℃~10℃","temp6":"0℃~0℃","tempF1":"","tempF2":"","tempF3":"","tempF4":"","tempF5":"","tempF6":"","weather1":"晴","weather2":"晴转多云","weather3":"阴转多云","weather4":"晴","weather5":"晴转多云","weather6":"晴","week":"星期二","wind1":"北风转无持续风向","wind2":"微风","wind3":"北风","wind4":"北风转无持续风向","wind5":"微风","wind6":"微风"},
    "realtime":
        {"SD":"27%","WD":"西南风","WS":"2级","WSE":"","city":"","cityid":"101010100","isRadar":"1","radar":"JC_RADAR_AZ9010_JB","temp":"16","time":"13:55","weather":"多云"},
    "aqi":
        {"city":"北京","city_id":"101010100","pub_time":"2015-04-13 12:00","aqi":"38","pm25":"25","pm10":"30","so2":"5","no2":"16","src":"中国环境监测总站","spot":""},
    "index":
        [
            {"code":"fs","details":"属中等强度紫外辐射天气，外出时应注意防护，建议涂擦SPF指数高于15，PA+的防晒护肤品。","index":"中等","name":"防晒指数","otherName":""},
            {"code":"ct","details":"天气冷，建议着棉服、羽绒服、皮夹克加羊毛衫等冬季服装。年老体弱者宜着厚棉衣、冬大衣或厚羽绒服。","index":"冷","name":"穿衣指数","otherName":""},
            {"code":"yd","details":"天气较好，但风力较大，推荐您进行室内运动，若在户外运动请注意避风保暖。","index":"较适宜","name":"运动指数","otherName":""},
            {"code":"xc","details":"较适宜洗车，未来一天无雨，风力较小，擦洗一新的汽车至少能保持一天。","index":"较适宜","name":"洗车指数","otherName":""},
            {"code":"ls","details":"天气不错，适宜晾晒。赶紧把久未见阳光的衣物搬出来吸收一下太阳的味道吧！","index":"适宜","name":"晾晒指数","otherName":""}
        ],
    "today":
        {"cityCode":"101010100","date":"2015-04-13","humidityMax":74,"humidityMin":27,"precipitationMax":0.2,"precipitationMin":0,"tempMax":18,"tempMin":4,"weatherEnd":"多云","weatherStart":"阵雨","windDirectionEnd":"西南风","windDirectionStart":"东北风","windMax":4,"windMin":1},
    "yestoday":
        {"cityCode":"101010100","date":"2015-04-12","humidityMax":100,"humidityMin":30,"precipitationMax":16,"precipitationMin":0,"tempMax":11,"tempMin":4,"weatherEnd":"阵雨","weatherStart":"小雨","windDirectionEnd":"北风","windDirectionStart":"东风","windMax":5,"windMin":0}
}

#endif
