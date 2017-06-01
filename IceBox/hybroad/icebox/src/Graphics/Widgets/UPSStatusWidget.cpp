#include "UPSStatusWidget.h"
#include "PlatformAPI.h"

#define REFRESH_INTERVEL 300000
#define DEFAULT_CHARGE_TIME 600000

namespace IBox {

UPSStatusWidget::UPSStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h)
    , mNextRefreshTime(0), mCurrentState(-1), mChargeStatus(-1)
{
    mUPSImg = _CreateImageView(5, 15, 30, 35);
    mUPSImg->setSource(UPS_ICON_NULL).inval(0);
}

UPSStatusWidget::~UPSStatusWidget()
{
    if (mUPSImg) {
        mUPSImg->detachFromParent();
        delete mUPSImg;
    }
}

int 
UPSStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mUPSImg);
    }
    return 0;
}

int 
UPSStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mRefreshFlg != REFRESH_FORCE_ONCE) {
        if (mNextRefreshTime > clocktime)
            return 0;
    } else
        mRefreshFlg = REFRESH_UNDEFINED;

    mNextRefreshTime = clocktime + REFRESH_INTERVEL;

    if (mChargeStatus == UPS_CHARGE_OVER) {
        mChargeStatus = -1;
        if (mCurrentState != e_WARNING) {
            mCurrentState = e_WARNING;
            mUPSImg->setSource(UPS_ICON_WARNING);
            goto END1;
        }
    } else {
        int status = Platform().getChargeStatus();
        if (status < 0) {
            if (mCurrentState == e_NULL)
                goto END0;
            mUPSImg->setSource(UPS_ICON_NULL);
            mCurrentState = e_NULL; 
        } else if (0 == status) {
            if (mCurrentState == e_GOOD)
                goto END0;
            mUPSImg->setSource(UPS_ICON_GOOD);
            mCurrentState = e_GOOD; 
        } else {
            //charging
            if (mCurrentState == e_ERROR)
                goto END0;
            if (clocktime > DEFAULT_CHARGE_TIME) {
                mUPSImg->setSource(UPS_ICON_ERROR);
                mCurrentState = e_ERROR;
            }
        }
    }

    mUPSImg->setSource(UPS_ICON_ERROR);
END1:
    mUPSImg->inval(0);
    mUPSImg->setVisibleP(true);
    return 1;
END0:
    return 0;
}

}
