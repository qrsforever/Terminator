#include "TimeWidget.h"

namespace IBox {

TimeWidget::TimeWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h)
{
    mTimeTxt = _CreateTextView(3, 30, 142, 80);
    mTimeTxt->setFontSize(66);
    mTimeTxt->setBGColor(0, 0, 0, 0);
    mTimeTxt->setFontColor(0xff, 0xff, 0xff, 0xff);
    mTimeTxt->setAlignStyle(TextView::ALIGN_CENTER);
    mTimeTxt->setText("08:00").inval(0);

    mLineImg = _CreateImageView(158, 30, 1, 80);
    mLineImg->setSource(LINE_ICON_V).inval(0);
}

TimeWidget::~TimeWidget()
{
    if (mTimeTxt) {
        mTimeTxt->detachFromParent();
        delete mTimeTxt;
    }
    if (mLineImg) {
        mLineImg->detachFromParent();
        delete mLineImg;
    }
}

int 
TimeWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mTimeTxt);
        parent->attachChildToFront(mLineImg);
    }
}

bool 
TimeWidget::isSameTime(const SysTime::DateTime& dt)
{
    return (dt.mHour == mHour && dt.mMinute == mMinute);
}

void 
TimeWidget::syncTime(const SysTime::DateTime& dt)
{
    mHour = dt.mHour;
    mMinute = dt.mMinute;
}

int 
TimeWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (isSameTime(dt))
        return 0;
    syncTime(dt);

    char txt[16] = { 0 };
    sprintf(txt, "%02d:%02d", mHour, mMinute);
    mTimeTxt->setText(txt).inval(0);
    mTimeTxt->setVisibleP(true);
    mLineImg->setVisibleP(true);
    return 1;
}

}
