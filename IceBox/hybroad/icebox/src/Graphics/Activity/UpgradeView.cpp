#include "GraphicsAssertions.h"
#include "UpgradeView.h"
#include "ImageView.h"
#include "TextView.h"
#include "Canvas.h"
#include "ResourcePathDefine.h"

#define GET_UPGRADE_REQUEST "收到用户升级请求,"
#define UDISK_FIND_VERSION  "检测U盘有升级文件,"
#define NEW_VERSIONNUMBER   "最新版本号 "
#define PRESS_OK_UPGRADE    "请按C键确认升级" 
#define UPGRADING_VERSION   "升级中,新版本 "
#define BURNING_VERSION     "烧写新版本 "

#define UPGRADE_SUCCESS     "新版本升级成功," 
#define NEW_VERSIONNUMBER2  "版本号 "
#define PRESS_OK_REBOOT     "请按C键确认重启" 

namespace IBox {

UpgradeView::UpgradeView(int x, int y, int w, int h)
    : mFontSize(20), mBarHeight(25),  mImage(0), mVersionNumber("")
    , mNowSize(0), mTotalSize(0), mProgress(0), mMesage(""), mShowWhat(-1)
{
    setLoc(x, y);
    setSize(w, h);
    mImage = new ImageView(220 + 5, 10, 80, 95);
}

UpgradeView::~UpgradeView()
{
    if (mImage) {
        delete mImage;
        mImage = 0;
    }
}

UpgradeView& 
UpgradeView::promptVersionMessage(const char* versionNumber, bool localUpgrade)
{
    if (mShowWhat != e_UpgradePromptVersion) {
        mImage->setVisibleP(true);
        mImage->setSource(UPGRADE_ICON_PROMPT).inval(0);
        attachChildToFront(mImage);
        mShowWhat = e_UpgradePromptVersion;
        mVersionNumber = versionNumber;
        mLocalUpgrade = localUpgrade;
    }
    return *this;
}

UpgradeView& 
UpgradeView::promptSuccessMessage(const char* success)
{
    if (mShowWhat != e_UpgradePromptSuccess) {
        mImage->setVisibleP(true);
        mImage->setSource(UPGRADE_ICON_PROMPT).inval(0);
        attachChildToFront(mImage);
        mShowWhat = e_UpgradePromptSuccess;
    }
    return *this;
}

UpgradeView& 
UpgradeView::setDownloadProgress(int progress)
{
    mProgress = progress;
    if (mShowWhat != e_UpgradeDProgress) {
        mImage->setVisibleP(false);
        mImage->detachFromParent();
        mShowWhat = e_UpgradeDProgress;
    }
    return *this;
}

UpgradeView& 
UpgradeView::setBurningProgress(int progress)
{
    mProgress = progress;
    if (mShowWhat != e_UpgradeBProgress) {
        mImage->setVisibleP(false);
        mImage->detachFromParent();
        mShowWhat = e_UpgradeBProgress;
    }
    return *this;
}

void 
UpgradeView::onDraw(Canvas* canvas)
{
    cairo_text_extents_t extents;
    double x = 0, y = 0;
    std::string tmpstr("");

    cairo_t* cr = canvas->mCairo;

    cairo_save(cr); 
    {
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);                  
        cairo_paint(cr);
        cairo_set_font_size(cr, mFontSize);

        if (mShowWhat == e_UpgradePromptVersion) {
            cairo_translate (cr, 0, 15);
            cairo_set_source_rgb (cr, 1, 1, 1);        

            if (mLocalUpgrade)
                tmpstr = UDISK_FIND_VERSION;
            else 
                tmpstr = GET_UPGRADE_REQUEST;
            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, tmpstr.c_str());
            cairo_translate (cr, 0, -extents.y_bearing + 15);

            tmpstr = NEW_VERSIONNUMBER;
            tmpstr.append(mVersionNumber);

            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, tmpstr.c_str());
            cairo_translate (cr, 0, -extents.y_bearing + 15);

            cairo_set_source_rgb (cr, 1, 1, 0);
            cairo_text_extents (cr, PRESS_OK_UPGRADE, &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, PRESS_OK_UPGRADE);
        } else if (mShowWhat == e_UpgradePromptSuccess) {
            cairo_translate (cr, 0, 15);
            cairo_set_source_rgb (cr, 1, 1, 1);        
            cairo_text_extents (cr, UPGRADE_SUCCESS, &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, UPGRADE_SUCCESS);
            cairo_translate (cr, 0, -extents.y_bearing + 15);

            tmpstr = NEW_VERSIONNUMBER2;
            tmpstr.append(mVersionNumber);

            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, tmpstr.c_str());
            cairo_translate (cr, 0, -extents.y_bearing + 15);

            cairo_set_source_rgb (cr, 1, 1, 0);
            cairo_text_extents (cr, PRESS_OK_REBOOT, &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, PRESS_OK_REBOOT);
        } else {
            //Progress
            x = 15.0;
            y = height() - mBarHeight - 20;

            if (mShowWhat == e_UpgradeDProgress)
                tmpstr = UPGRADING_VERSION;
            else 
                tmpstr = BURNING_VERSION;
            tmpstr.append(mVersionNumber);

            cairo_set_source_rgb (cr, 1, 1, 1);        
            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, x + 5, -extents.y_bearing + 30);
            cairo_show_text (cr, tmpstr.c_str());

            cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);                  
            cairo_rectangle (cr, x, y, (320-2*x), mBarHeight);
            cairo_fill (cr);   

            cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);                  
            cairo_rectangle (cr, x, y + 1.5, (320-2*x) * mProgress / 100 , mBarHeight - 3);
            cairo_fill (cr);   

            cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);                  
            char buff[32] = { 0 };
            sprintf(buff, "%d%%", mProgress);
            cairo_move_to (cr, x + 130, y + 20);
            cairo_show_text (cr, buff);
        }
    }
    cairo_restore(cr);
}

void
UpgradeView::onHandleInval(const Rect& r)
{
}

}
