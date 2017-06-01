#include "GraphicsAssertions.h"
#include "FanStatusWidget.h"
#include "PlatformAPI.h"
#include <math.h>

#define REFRESH_INTERVEL 120000
#define ERROR_BAND 20

namespace IBox {

FanStatusWidget::FanStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h),  mRefSpeed(100), mNextRefreshTime(0), mCurrentState(-1)
{
    mFanImg = _CreateImageView(5, 15, 30, 35);
    mFanImg->setSource(FAN_ICON_NULL).inval(0);
}

FanStatusWidget::~FanStatusWidget()
{
    if (mFanImg) {
        mFanImg->detachFromParent();
        delete mFanImg;
    }
}

int 
FanStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mFanImg);
    }
    return 0;
}

int 
FanStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mNextRefreshTime > clocktime)
        return 0;
    mNextRefreshTime = clocktime + REFRESH_INTERVEL;

    int speed = Platform().getFanSpeed();
    GRAPHICS_LOG("Fan speed: %d vs %d\n", speed, mRefSpeed);

    if (0 == speed) {
        if (mCurrentState == e_NULL)
            goto END0;
        mFanImg->setSource(FAN_ICON_NULL).inval(0);
    } else {
        int band = abs(mRefSpeed - speed);
        if (band > ERROR_BAND) {
            if (mCurrentState == e_ERROR)
                goto END0;
            mFanImg->setSource(FAN_ICON_ERROR).inval(0);
        } else {
            if (mCurrentState == e_GOOD)
                goto END0;
            mFanImg->setSource(FAN_ICON_GOOD).inval(0);
        }
    }
    mFanImg->setVisibleP(true);
    return 1;

END0:
    return 0;
}

}
