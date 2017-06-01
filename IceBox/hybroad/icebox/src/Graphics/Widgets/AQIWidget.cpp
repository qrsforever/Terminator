#include "AQIWidget.h"
#include "WeatherManager.h"

namespace IBox {

AQIWidget::AQIWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mAQI(-1)
{
    mAQIImg = _CreateImageView(20, 35, 60, 20);
    mAQIImg->setSource(AQI_ICON_GOOD).inval(0);

    mAQITxt = _CreateTextView(83, 5, 70, 50);
    mAQITxt->setFontSize(48);
    mAQITxt->setBGColor(0, 0, 0, 0);
    mAQITxt->setFontColor(0x99, 0x99, 0x99, 255);
    mAQITxt->setAlignStyle(TextView::ALIGN_CENTER_BOTTOM);
    mAQITxt->setText("100").inval(0);
}

AQIWidget::~AQIWidget()
{
    if (mAQIImg) {
        mAQIImg->detachFromParent();
        delete mAQIImg;
    }
    if (mAQITxt) {
        mAQITxt->detachFromParent();
        delete mAQITxt;
    } 
}

int 
AQIWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mAQIImg);
        parent->attachChildToFront(mAQITxt);
    }
    return 0;
}

int 
AQIWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    int aqi = WeatherManager::self().getAQI();
    if (aqi == mAQI)
        return 0;
    mAQI = aqi;
    
    if (mAQI < 110)
        mAQIImg->setSource(AQI_ICON_EXCELLENT);
    else if (mAQI > 200)
        mAQIImg->setSource(AQI_ICON_POOR);
    else
        mAQIImg->setSource(AQI_ICON_GOOD);
    mAQIImg->inval(0);
    mAQIImg->setVisibleP(true);

    char txt[8] = { 0 };
    sprintf(txt, "%d", mAQI);
    mAQITxt->setText(txt).inval(0);
    mAQITxt->setVisibleP(true);
    return 1;
}

}
