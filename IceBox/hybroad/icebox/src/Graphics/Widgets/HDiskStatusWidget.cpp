#include "HDiskStatusWidget.h"
#include "DiskDeviceManager.h"

namespace IBox {

#define REFRESH_INTERVEL 180000
#define ALARM_PERCENT 5

HDiskStatusWidget::HDiskStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mNextRefreshTime(0), mCurrentState(-1)
{
    mHDiskImg = _CreateImageView(5, 15, 30, 35);
    if (HD0_WIDGET_ID == getID())
        mHDiskImg->setSource(HDISK0_ICON_NULL).inval(0);
    else
        mHDiskImg->setSource(HDISK1_ICON_NULL).inval(0);
}

HDiskStatusWidget:: ~HDiskStatusWidget()
{
    if (mHDiskImg) {
        mHDiskImg->detachFromParent();
        delete mHDiskImg;
    }
}

int
HDiskStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mHDiskImg);
    }
    return 0;
}

int 
HDiskStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mRefreshFlg != REFRESH_FORCE_ONCE) {
        if (mNextRefreshTime > clocktime)
            return 0;
    } else
        mRefreshFlg = REFRESH_UNDEFINED;

    mNextRefreshTime = clocktime + REFRESH_INTERVEL;

    DiskDevice* disk = 0;
    if (HD0_WIDGET_ID == getID()) {
        disk = Disk().getDiskDeviceByBusLabel(BUSLABEL_HDD0);
        if (!disk) {
            if (mCurrentState == e_NULL) 
                goto END0;
            mCurrentState = e_NULL;
            mHDiskImg->setSource(HDISK0_ICON_NULL);
            goto END1;
        }
        unsigned long long t_total = 0;
        unsigned long long t_avail = 0;
        disk->getAllSpace(&t_total, &t_avail);
        if (0 == t_total) {
            if (mCurrentState == e_ERROR) 
                goto END0;
            mCurrentState = e_ERROR;
            mHDiskImg->setSource(HDISK0_ICON_ERROR);
            goto END1;
        }
        if ((t_avail * 100 / t_total) < ALARM_PERCENT) {
            if (mCurrentState == e_WARNING)
                goto END0;
            mCurrentState = e_WARNING;
            mHDiskImg->setSource(HDISK0_ICON_WARNING);
            goto END1;
        }
        if (mCurrentState == e_GOOD)
            goto END0;
        mCurrentState = e_GOOD;
        mHDiskImg->setSource(HDISK0_ICON_GOOD);
    } else {
        disk = Disk().getDiskDeviceByBusLabel(BUSLABEL_HDD1);
        if (!disk) {
            if (mCurrentState == e_NULL) 
                goto END0;
            mCurrentState = e_NULL;
            mHDiskImg->setSource(HDISK1_ICON_NULL);
            goto END1;
        }
        unsigned long long t_total = 0;
        unsigned long long t_avail = 0;
        disk->getAllSpace(&t_total, &t_avail);
        if (0 == t_total) {
            if (mCurrentState == e_ERROR) 
                goto END0;
            mCurrentState = e_ERROR;
            mHDiskImg->setSource(HDISK1_ICON_ERROR);
            goto END1;
        }
        if ((t_avail * 100 / t_total) < ALARM_PERCENT) {
            if (mCurrentState == e_WARNING)
                goto END1;
            mCurrentState = e_WARNING;
            mHDiskImg->setSource(HDISK1_ICON_WARNING);
            goto END1;
        }
        if (mCurrentState == e_GOOD)
            goto END0;
        mCurrentState = e_GOOD;
        mHDiskImg->setSource(HDISK1_ICON_GOOD);
    }
END1:
    mHDiskImg->inval(0);
    mHDiskImg->setVisibleP(true);
    return 1;
END0:
    return 0;
}

}
