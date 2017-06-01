#ifndef __WeatherWidget__H_
#define __WeatherWidget__H_ 

#include "Widget.h"

namespace IBox { 

class WeatherWidget : public Widget {
public:
    WeatherWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~WeatherWidget();

private:
    void layoutMix();
    int mixAlert(unsigned int t);

private:
    MixNullView* mMix1;
    ImageView* mWFeelImg;
    ImageView* mCSmallImg;
    TextView*  mTRangeTxt;

    MixNullView* mMix2;
    ImageView* mTScaleImg;
    ImageView* mCBigImg;
    TextView*  mTempTxt;

    enum {
        e_Mix1,
        e_Mix2 ,
    };

    int mImgNO;
    int mMinT;
    int mMaxT;
    int mNowT;
    unsigned int mNextClockTime;
};

}

#endif
