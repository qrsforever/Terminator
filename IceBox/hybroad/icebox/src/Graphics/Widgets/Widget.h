#ifndef __WIDGET__H_
#define __WIDGET__H_ 

#include <time.h>
#include "ImageView.h"
#include "TextView.h"
#include "SysTime.h"
#include "View.h"
#include "MessageHandler.h"
#include "ResourcePathDefine.h"

#define FAN_WIDGET_ID 0
#define HD0_WIDGET_ID 1
#define HD1_WIDGET_ID 2
#define SD_WIDGET_ID 3
#define USB0_WIDGET_ID 4
#define USB1_WIDGET_ID 5
#define WIFI_WIDGET_ID 6
#define UPS_WIDGET_ID 7

#define TIME_WIDGET_ID 12
#define DATE_WIDGET_ID 13
#define AQI_WIDGET_ID  14
#define WEATHER_WIDGET_ID 15

#define PROMPT_WIDGET_ID 16

namespace IBox {

#define REFRESH_UNDEFINED  0
#define REFRESH_FORCE_ONCE 1

#define UPS_CHARGE_FULL 100
#define UPS_CHARGE_OVER 101

class Widget {
public:
    int getID() { return mID; };
    int X() { return mX; }
    int Y() { return mY; }
    int W() { return mW; }
    int H() { return mH; }

    virtual int attachBy(View* parent) = 0;
    virtual int refresh(unsigned int, const SysTime::DateTime&, int) = 0;

    void setRefreshFlag(int flg) { mRefreshFlg = flg; }
    void setMessageHandler(MessageHandler* h) { mHandler = h; }

protected:
    Widget(int id, int x, int y, int w, int h);
    ~Widget();

    enum {
        e_NULL = 0,
        e_GOOD = 1,
        e_WARNING = 2,
        e_ERROR = 3, 
    };

    class MixNullView : public View {
        public:
            MixNullView(int x, int y, int w, int h);
            ~MixNullView();
            void hide() { setVisibleP(false); }
            void show() { inval(0); setVisibleP(true); }
    };

    ImageView* _CreateImageView(int x, int y, int w, int h);
    TextView* _CreateTextView(int x, int y, int w, int h);

protected:
    MessageHandler* mHandler;
    int mRefreshFlg;

private:
    int mID;
    int mX;
    int mY;
    int mW;
    int mH;
};

}

#endif
