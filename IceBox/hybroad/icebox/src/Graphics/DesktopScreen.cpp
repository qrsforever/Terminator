#include "DesktopScreen.h"
#include "Canvas.h"
#include "SysTime.h"
#include "ImageView.h"
#include "TextView.h"
#include "UpgradeView.h"
#include "AuthCodeView.h"
#include "DiskFormatView.h"
#include "DiskCopyView.h"
#include "EasysetupView.h"
#include <vector>

namespace IBox {

DesktopScreen::DesktopScreen(Canvas* c, int w, int h, int z) 
    : LCDScreen(c, w, h, z), mDiskFormatLayer(0), mEasysetupLayer(0)
    , mBGImage(0), mUpgradeLayer(0), mAuthCodeLayer(0), mDiskCopyLayer(0)
{ 
    setVisibleP(true);
    mMainLayer = new View();
    mMainLayer->setLoc(0, 0);
    mMainLayer->setSize(w, h);
    attachChildToFront(mMainLayer);
    mUpgradeLayer = new UpgradeView(0, 60, width(), 120); 
}

DesktopScreen::~DesktopScreen()
{ 
    if (mMainLayer)
        delete mMainLayer;
    if (mUpgradeLayer)
        delete mUpgradeLayer;
}

void 
DesktopScreen::setBackgroud(const char* imgpath)
{
    if (!mBGImage) {
        mBGImage = new ImageView(locX(), locY(), width(), height());
        attachChildToBack(mBGImage);
    }

    mBGImage->setVisibleP(false);

    if (imgpath) {
        mBGImage->setVisibleP(true);
        mBGImage->setSource(imgpath).inval(0);
    }
}

void 
DesktopScreen::showMainLayer()
{
    return mMainLayer->setVisibleP(true);
}

void 
DesktopScreen::hideMainLayer()
{
    return mMainLayer->setVisibleP(false);
}

void 
DesktopScreen::showUpgradeLayer()
{
    attachChildToFront(mUpgradeLayer);
    mUpgradeLayer->setVisibleP(true);
}

void 
DesktopScreen::hideUpgradeLayer()
{
    mUpgradeLayer->detachFromParent();
    mUpgradeLayer->setVisibleP(false);
    return ;
}

void 
DesktopScreen::showIdentifyCode(const char* code)
{
    if (!mAuthCodeLayer) {
        mAuthCodeLayer = new AuthCodeView(0, 60, width(), 120); 
        mAuthCodeLayer->setVisibleP(true);
        attachChildToFront(mAuthCodeLayer);
    }
    mAuthCodeLayer->setCodeString(code).inval(0);
    return ;
}

int 
DesktopScreen::getIdentifyCode()
{
    if (!mAuthCodeLayer)
        return -1;
   return mAuthCodeLayer->getCodeNumber();
}

void
DesktopScreen::clearIdentifyCode()
{
    if (mAuthCodeLayer) {
        mAuthCodeLayer->detachFromParent();
        mAuthCodeLayer->setVisibleP(false);
        delete mAuthCodeLayer;
        mAuthCodeLayer = 0;
    }
    return ;
}

void
DesktopScreen::showDiskCopyLayer()
{
    if(!mDiskCopyLayer) {
        mDiskCopyLayer = new DiskCopyView(0, 60, width(), 120);
        mDiskCopyLayer->setVisibleP(true);
        attachChildToFront(mDiskCopyLayer);
    }
}

DiskCopyView*
DesktopScreen::getDiskCopyView()
{
    if (!mDiskCopyLayer)
        showDiskCopyLayer();

    return mDiskCopyLayer;
}

void
DesktopScreen::clearDiskCopyLayer()
{
    if(mDiskCopyLayer) { 
        mDiskCopyLayer->detachFromParent();
        mDiskCopyLayer->setVisibleP(false);
        delete mDiskCopyLayer;
        mDiskCopyLayer = 0;
    }
}

void 
DesktopScreen::showDiskFormatLayer()
{
    if (!mDiskFormatLayer) {
        mDiskFormatLayer = new DiskFormatView(0, 60, width(), 120); 
        mDiskFormatLayer->setVisibleP(true);
        attachChildToFront(mDiskFormatLayer);
    }
}

DiskFormatView*
DesktopScreen::getDiskFormatView() 
{ 
    if (!mDiskFormatLayer)
        showDiskFormatLayer();

    return mDiskFormatLayer; 
}

void 
DesktopScreen::clearDiskFormatLayer()
{
    if (mDiskFormatLayer) {
        mDiskFormatLayer->detachFromParent();
        mDiskFormatLayer->setVisibleP(false);
        delete mDiskFormatLayer;
        mDiskFormatLayer = 0;
    }
}

void 
DesktopScreen::showEasysetupLayer()
{
    if (!mEasysetupLayer) {
        mEasysetupLayer = new EasysetupView(0, 60, width(), 120); 
        mEasysetupLayer->setVisibleP(true);
        attachChildToFront(mEasysetupLayer);
    }
}

EasysetupView* 
DesktopScreen::getEasysetupView()
{
    if (!mEasysetupLayer)
        showEasysetupLayer();

    return mEasysetupLayer; 
}

void 
DesktopScreen::clearEasysetupLayer()
{
    if (mEasysetupLayer) {
        mEasysetupLayer->detachFromParent();
        mEasysetupLayer->setVisibleP(false);
        delete mEasysetupLayer;
        mEasysetupLayer = 0;
    }
}

void 
DesktopScreen::addWidget(Widget* widget)
{
    if (!widget)
        return;
    mWidgets.insert(std::make_pair(widget->getID(), widget));
    widget->attachBy(mMainLayer);
    return ;
}

Widget*
DesktopScreen::getWidget(int id)
{
    std::map<int, Widget*>::iterator it = mWidgets.find(id);

    if (it != mWidgets.end()) 
        return it->second;
    return 0;
}

int 
DesktopScreen::refreshAllWidgets()
{
    std::map<int, Widget*>::iterator it = mWidgets.begin();
    int refreshed = 0;
    SysTime::DateTime dt;
    SysTime::GetDateTime(&dt);
    while (it != mWidgets.end()) {
        refreshed |= it->second->refresh(SysTime::GetMSecs(), dt, 0);
        ++it;
    }
    return refreshed;
}

void 
DesktopScreen::onDraw(Canvas* canvas)
{
    cairo_t* cr = canvas->mCairo;

    cairo_save(cr);
    {
        cairo_rectangle(cr, 0, 0, width(), height());
        cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
        cairo_fill(cr);
    }
    cairo_restore(cr);
}

}
