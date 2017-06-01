#include "WifiStatusWidget.h"

namespace IBox {

WifiStatusWidget::WifiStatusWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h)
{
    mWifiImg = _CreateImageView(5, 15, 30, 35);
    mWifiImg->setSource(DESKTOP_ICON_DIR"WIFI-Null1.png").inval(0);
}

WifiStatusWidget::~WifiStatusWidget()
{
    if (mWifiImg) {
        mWifiImg->detachFromParent();
        delete mWifiImg;
    }
}

int 
WifiStatusWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mWifiImg);
    }
    return 0;
}

int 
WifiStatusWidget::refresh(unsigned int clocktime, const SysTime::DateTime&, int force)
{
    mWifiImg->setVisibleP(true);
    return 1;
}

}
