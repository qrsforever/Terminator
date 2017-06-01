#include "USBStatusWidget.h"
#include "DiskDeviceManager.h"

namespace IBox {

#define REFRESH_INTERVEL 180000
#define ALARM_PERCENT 3

USBStatusWidget::USBStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mNextRefreshTime(0), mCurrentState(-1)
{
    mUSBImg = _CreateImageView(5, 15, 30, 35);
    mUSBImg->setSource(USB_ICON_NULL).inval(0);
}

USBStatusWidget::~USBStatusWidget()
{
    if (mUSBImg) {
        mUSBImg->detachFromParent();
        delete mUSBImg;
    }
}

int 
USBStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mUSBImg);
    }
    return 0;
}

int 
USBStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (mRefreshFlg != REFRESH_FORCE_ONCE) {
        if (mNextRefreshTime > clocktime)
            return 0;
    } else
        mRefreshFlg = REFRESH_UNDEFINED;

    mNextRefreshTime = clocktime + REFRESH_INTERVEL;

    unsigned long long t_total = 0;
    unsigned long long t_avail = 0;
    DiskDevice* disk = 0;
    switch (getID()) {
    case USB0_WIDGET_ID:
        disk = Disk().getDiskDeviceByBusLabel(BUSLABEL_USB0);
        break;
    case USB1_WIDGET_ID:
        disk = Disk().getDiskDeviceByBusLabel(BUSLABEL_USB1);
        break;
    default:
        return 0;
    }
    if (!disk) {
        if (mCurrentState == e_NULL) 
            goto END0;
        mCurrentState = e_NULL;
        mUSBImg->setSource(USB_ICON_NULL);
        goto END1;
    }
    disk->getAllSpace(&t_total, &t_avail);
    if (0 == t_total) {
        if (mCurrentState == e_ERROR) 
            goto END0;
        mCurrentState = e_ERROR;
        mUSBImg->setSource(USB_ICON_ERROR);
        goto END1;
    }
    if ((t_avail * 100 / t_total) < ALARM_PERCENT) {
        if (mCurrentState == e_WARNING)
            goto END0;
        mCurrentState = e_WARNING;
        mUSBImg->setSource(USB_ICON_WARNING);
        goto END1;
    }
    if (mCurrentState == e_GOOD)
        goto END0;
    mCurrentState = e_GOOD;
    mUSBImg->setSource(USB_ICON_GOOD);
END1:
    mUSBImg->inval(0);
    mUSBImg->setVisibleP(true);
    return 1;
END0:
    return 0;
}

}
