#include "WeatherWidget.h"
#include "WeatherManager.h"

namespace IBox {

#define MIX_ALTER_TIME 5000

WeatherWidget::WeatherWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h)
    , mNextClockTime(0), mMaxT(-1), mMinT(-1), mNowT(-1), mImgNO(-1)
{
    mMix1 = new MixNullView(x, y, w, h);
    mMix2 = new MixNullView(x, y, w, h);
    layoutMix();
}

WeatherWidget::~WeatherWidget()
{
    if (mMix1) {
        mMix1->detachFromParent();
        delete mWFeelImg;
        delete mCSmallImg;
        delete mTRangeTxt;
        delete mMix1;
    }
    if (mMix2) {
        mMix2->detachFromParent();
        delete mTScaleImg;
        delete mCBigImg;
        delete mTempTxt;
        delete mMix2;
    }
}

void 
WeatherWidget::layoutMix()
{
    if (mMix1) {
        mWFeelImg = new ImageView(12, 40, 60, 60);
        mCSmallImg = new ImageView(142, 80, 10, 10);
        mTRangeTxt = new TextView(67, 40, 80, 60);
        mTRangeTxt->setFontSize(20);
        mTRangeTxt->setFontColor(0xff, 0xff, 0xff, 0xff);
        mTRangeTxt->setAlignStyle(TextView::ALIGN_CENTER_BOTTOM);

        mMix1->attachChildToFront(mWFeelImg);
        mMix1->attachChildToFront(mCSmallImg);
        mMix1->attachChildToFront(mTRangeTxt);

        mWFeelImg->setVisibleP(true);
        mCSmallImg->setVisibleP(true);
        mTRangeTxt->setVisibleP(true);
        mCSmallImg->setSource(CD_ICON_SMALL).inval(0);
    }

    if (mMix2) {
        mTScaleImg = new ImageView(12, 45, 20, 50);
        mCBigImg = new ImageView(125, 55, 25, 25);
        mTempTxt = new TextView(40, 35, 80, 60);
        mTempTxt->setFontSize(56);
        mTempTxt->setFontColor(0xff, 0xff, 0xff, 0xff);
        mTempTxt->setAlignStyle(TextView::ALIGN_CENTER_BOTTOM);

        mMix2->attachChildToFront(mTScaleImg);
        mMix2->attachChildToFront(mCBigImg);
        mMix2->attachChildToFront(mTempTxt);

        mTScaleImg->setVisibleP(true);
        mCBigImg->setVisibleP(true);
        mTempTxt->setVisibleP(true);
        mTScaleImg->setSource(TEMP_ICON_NORMAL).inval(0);
        mCBigImg->setSource(CD_ICON_BIG).inval(0);
    }
}

int 
WeatherWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mMix1);
        parent->attachChildToFront(mMix2);
    }
    return 0;
}

int 
WeatherWidget::mixAlert(unsigned int t)
{
    static int sflag = e_Mix2;

    if (e_Mix1 == sflag) {
        mMix1->hide();
        mMix2->show();
        sflag = e_Mix2;
    } else {
        mMix1->show();
        mMix2->hide();
        sflag = e_Mix1;
    }
    mNextClockTime = t + MIX_ALTER_TIME;

    return sflag;
}

int 
WeatherWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mNextClockTime > clocktime)
        return 0;

    int sflg = mixAlert(clocktime);
    char txt[32] = { 0 };

    WeatherManager& wm = Weather();
    switch (sflg) {
    case e_Mix1:
        if (mImgNO != wm.getImgNO()) {
            mImgNO = wm.getImgNO();
            char path[128] = { 0 };
            if (7 < dt.mHour && dt.mHour <= 19) {
                snprintf(path, 127, "%sday_%02d.png", DESKTOP_ICON_DIR, mImgNO);
                mWFeelImg->setSource(path).inval(0);
            } else {
                snprintf(path, 127, "%snight_%02d.png", DESKTOP_ICON_DIR, mImgNO);
                mWFeelImg->setSource(path).inval(0);
            }
        }
        if (mMinT != wm.getMinT() || mMaxT != wm.getMaxT()) {
            mMinT = wm.getMinT();
            mMaxT = wm.getMaxT();
            sprintf(txt, "%d~%d", mMinT, mMaxT);
            mTRangeTxt->setText(txt).inval(0);
        }
        break;
    case e_Mix2:
        if (mNowT != wm.getNowT()) {
            mNowT = wm.getNowT();
            sprintf(txt, "%d", mNowT);
            mTempTxt->setText(txt).inval(0);
        }
        break;
    default:
        return 0;
    }

    return 1;
}

}
