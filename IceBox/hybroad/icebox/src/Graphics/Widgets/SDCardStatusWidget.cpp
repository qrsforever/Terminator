#include "SDCardStatusWidget.h"
#include "DiskDeviceManager.h"

namespace IBox {

#define REFRESH_INTERVEL 180000
#define ALARM_PERCENT 3

SDCardStatusWidget::SDCardStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mNextRefreshTime(0), mCurrentState(-1)
{
    mSDCardImg = _CreateImageView(5, 15, 30, 35);
    mSDCardImg->setSource(SDCARD_ICON_NULL).inval(0);
}

SDCardStatusWidget::~SDCardStatusWidget()
{
    if (mSDCardImg) {
        mSDCardImg->detachFromParent();
        delete mSDCardImg;
    }
}

int 
SDCardStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mSDCardImg);
    }
    return 0;
}

int 
SDCardStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mRefreshFlg != REFRESH_FORCE_ONCE) {
        if (mNextRefreshTime > clocktime)
            return 0;
    } else
        mRefreshFlg = REFRESH_UNDEFINED;

    mNextRefreshTime = clocktime + REFRESH_INTERVEL;

    unsigned long long t_total = 0;
    unsigned long long t_avail = 0;
    DiskDevice* disk = Disk().getDiskDeviceByBusLabel(BUSLABEL_SDCARD);
    if (!disk) {
        if (mCurrentState == e_NULL) 
            goto END0;
        mCurrentState = e_NULL;
        mSDCardImg->setSource(SDCARD_ICON_NULL);
        goto END1;
    }
    disk->getAllSpace(&t_total, &t_avail);
    if (0 == t_total) {
        if (mCurrentState == e_ERROR) 
            goto END0;
        mCurrentState = e_ERROR;
        mSDCardImg->setSource(SDCARD_ICON_ERROR);
        goto END1;
    }
    if ((t_avail * 100 / t_total) < ALARM_PERCENT) {
        if (mCurrentState == e_WARNING)
            goto END0;
        mCurrentState = e_WARNING;
        mSDCardImg->setSource(SDCARD_ICON_WARNING);
        goto END1;
    }
    if (mCurrentState == e_GOOD)
        goto END0;
    mCurrentState = e_GOOD;
    mSDCardImg->setSource(SDCARD_ICON_GOOD);
END1:
    mSDCardImg->inval(0);
    mSDCardImg->setVisibleP(true);
    return 1;
END0:
    return 0;
}

}
