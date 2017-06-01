#include "GraphicsAssertions.h"
#include "DiskCopyView.h"
#include "ImageView.h"
#include "TextView.h"
#include "Canvas.h"
#include "ResourcePathDefine.h"

#define  HAVE_UDISK             "已插入U盘："
#define  UDISK_CAPACITY         "容量："
#define  SYSTEM_IS_SCANNING     "系统正在扫描"
#define  SCAN_HAVE_FINISHED     "扫描完成!"
#define  AVAILABLE_SPACE        "剩余空间："
//#define  HAVE_USED_SPACE        "已用空间："
#define  PRESS_OK_DISKCOPY      "请按C键完成一键导入"
#define  DISK_HAVENOT_SPACE     "云盘空间不足，无法全部导入"
#define  DISK_IS_COPYING        "正在导入U盘数据"
#define  DISK_HAVE_PULLOUT      "U盘已移除"
#define  DISK_SCAN_ERROR        "扫描发生错误"

namespace IBox {

DiskCopyView::DiskCopyView(int x, int y, int w, int h)
    : mImage(0), mImageOK(0), mFontSize(20), mBarHeight(25), mCopyProgress(0), mScanProgress(0)
    , mShowWhat(-1), mTotalSize(""), mAvailSize(""), mUName("")
    , mTotal(0), mAvail(0)
{
    setLoc(x, y);
    setSize(w, h);
    mImage = new ImageView(220+5, 10, 80, 95);
    mImageOK = new ImageView(220+5, 10, 80, 95);
}

DiskCopyView::~DiskCopyView()
{
    if (mImage) {
        mImage->detachFromParent();
        delete mImage;
        mImage = 0;
    }
    if (mImageOK) {
        mImageOK->detachFromParent();
        delete mImageOK;
        mImageOK = 0;
    }
}

DiskCopyView& 
DiskCopyView::setCopyProgress(int progress)
{
    mCopyProgress = progress;
    if(mShowWhat != e_DiskCopyPromptCopyProgress) {
        mImage->setVisibleP(false);
        mImage->detachFromParent();
        mImageOK->setVisibleP(false);
        mImageOK->detachFromParent();
        mShowWhat = e_DiskCopyPromptCopyProgress;
    }
    return *this;
}

DiskCopyView& 
DiskCopyView::setScanProgress(int progress)
{
    mScanProgress = progress;
    if(mShowWhat != e_DiskCopyPromptScanProgress)
        mShowWhat = e_DiskCopyPromptScanProgress;
    return *this;
}

DiskCopyView& 
DiskCopyView::promptUNameMessage(std::string UName, float total, float avail)
{
    if(mShowWhat != e_DiskCopyPromptUName) {
        attachChildToFront(mImage);
        mImage->setSource(DISKCOPY_ICON_PROMPT).inval(0);
        mImage->setVisibleP(true);
        mShowWhat = e_DiskCopyPromptUName;
        mUName = UName;
        mTotal = total;
        mAvail = avail;
        char buff[25] = {0};

        sprintf(buff,"%.1fGB",mTotal);
        mTotalSize = buff;

        sprintf(buff,"%.1fGB",mAvail);
        mAvailSize = buff;

   //     mSpace = (mTotal - mAvail) / mTotal;
    }
    return *this;
}

DiskCopyView& 
DiskCopyView::promptScanMessage()
{
    if(mShowWhat != e_DiskCopyPromptScanFinish) {
        mImage->setVisibleP(true);
        mImage->setSource(DISKCOPY_ICON_PROMPT).inval(0);
        attachChildToFront(mImage); 
        mImageOK->setVisibleP(true);
        mImageOK->setSource(DISKCOPY_ICONG_PROMPT).inval(0);
        attachChildToFront(mImageOK);
        mShowWhat = e_DiskCopyPromptScanFinish;
    }
    return *this;
}

DiskCopyView& 
DiskCopyView::promptSpaceNotEnoughMessage()
{
    if(mShowWhat != e_DiskCopyPromptSpaceNotEnough) {
        mImage->setVisibleP(true);
        mImage->setSource(DISKCOPY_ICON_PROMPT).inval(0);
        attachChildToFront(mImage);
        mImageOK->setVisibleP(true);
        mImageOK->setSource(DISKCOPY_ICONG_PROMPT).inval(0);
        attachChildToFront(mImageOK);
        mShowWhat = e_DiskCopyPromptSpaceNotEnough;
    }
    return *this;
}

DiskCopyView&
DiskCopyView::promptUPullOut()
{
    if(mShowWhat!= e_DiskCopyPromptPullout) {
        mImage->setVisibleP(false);
        mImage->detachFromParent();
        mImageOK->setVisibleP(false);
        mImageOK->detachFromParent();
        mShowWhat = e_DiskCopyPromptPullout;
    }
    return *this;
}

DiskCopyView&
DiskCopyView::promptDiskScanError()
{
    if(mShowWhat!= e_DiskCopyPromptScanError) {
        mImage->setVisibleP(false);
        mImage->detachFromParent();
        mImageOK->setVisibleP(false);
        mImageOK->detachFromParent();
        mShowWhat = e_DiskCopyPromptScanError;
    }
    return *this;
}

void 
DiskCopyView::onDraw(Canvas* canvas)
{
    cairo_text_extents_t extents;
    double x = 0, y = 0;
    std::string tmpstr("");
    std::string tmp("");
    char* tmpper;
    cairo_t* cr = canvas->mCairo;

    cairo_save(cr);
    {
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_paint(cr);
        cairo_set_font_size(cr, mFontSize);
    
        if (mShowWhat == e_DiskCopyPromptUName || mShowWhat == e_DiskCopyPromptScanProgress) {
            cairo_translate(cr, 0, 15);
            cairo_set_source_rgb(cr, 1, 1, 1);
            tmpstr = HAVE_UDISK;
            tmpstr.append(mUName);

            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
            
            tmpstr = UDISK_CAPACITY;
            tmpstr.append(mTotalSize);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);

            tmpstr = SYSTEM_IS_SCANNING;
            switch (mScanProgress % 3) {
            case 0:
                tmpstr.append(" .");
                break;
            case 1:
                tmpstr.append(" . .");
                break;
            case 2:
                tmpstr.append(" . . .");
                break;
            }
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
        } else if (mShowWhat == e_DiskCopyPromptScanFinish) {
            cairo_set_source_rgb(cr, 1, 1, 1);
            tmpstr = SCAN_HAVE_FINISHED;
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
            
            tmpstr = mUName;
            tmp = UDISK_CAPACITY;
            tmpstr.append(tmp);
            tmpstr.append(mTotalSize);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);

            tmpstr = AVAILABLE_SPACE;
            tmpstr.append(mAvailSize);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);

            cairo_set_source_rgb(cr, 1, 1, 0);
            tmpstr = PRESS_OK_DISKCOPY;
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
        } else if(mShowWhat == e_DiskCopyPromptSpaceNotEnough) {

            cairo_set_font_size(cr,18);
            cairo_set_source_rgb(cr, 1, 1, 1);
            tmpstr = SCAN_HAVE_FINISHED;
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 10);
            
            tmpstr = mUName;
            tmp = UDISK_CAPACITY;
            tmpstr.append(tmp);
            tmpstr.append(mTotalSize);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 10);
            
            tmpstr = AVAILABLE_SPACE;
            tmpstr.append(mAvailSize);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 10);

            cairo_set_source_rgb(cr, 1, 1, 0);
            tmpstr = PRESS_OK_DISKCOPY;
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 10);
        
            tmpstr = DISK_HAVENOT_SPACE;
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 5, -extents.y_bearing);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 10);
        } else if(mShowWhat == e_DiskCopyPromptCopyProgress) {
            x = 15.0;
            y = height() - mBarHeight - 20;
            
            tmpstr = DISK_IS_COPYING;
            cairo_set_source_rgb (cr, 1, 1, 1);        
            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, x + 5, -extents.y_bearing + 30);
            cairo_show_text (cr, tmpstr.c_str());

            cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);                  
            cairo_rectangle (cr, x, y, (320-2*x), mBarHeight);
            cairo_fill (cr);   

            cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);                  
            cairo_rectangle (cr, x, y+1.5, (320-2*x) * mCopyProgress / 100 , mBarHeight - 3);
            cairo_fill (cr);   

            cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);                  
            char buff[32] = { 0 };
            sprintf(buff, "%d%%", mCopyProgress);
            cairo_move_to (cr, x + 130, y + 20);
            cairo_show_text (cr, buff);
        } else if(mShowWhat == e_DiskCopyPromptPullout) {
            tmpstr = DISK_HAVE_PULLOUT;
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_set_font_size(cr,25);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 90, -extents.y_bearing + 50);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
        } else if(mShowWhat == e_DiskCopyPromptScanError) {
            tmpstr = DISK_SCAN_ERROR;
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_set_font_size(cr,25);
            cairo_text_extents(cr, tmpstr.c_str(), &extents);
            cairo_move_to(cr, 90, -extents.y_bearing + 50);
            cairo_show_text(cr, tmpstr.c_str());
            cairo_translate(cr, 0, -extents.y_bearing + 15);
        }    
    }
    cairo_restore(cr);
}

void
DiskCopyView::onHandleInval(const Rect& r)
{
}

}
