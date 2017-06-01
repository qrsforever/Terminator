#include "GraphicsAssertions.h"
#include "DiskFormatView.h"
#include "TextView.h"
#include "Canvas.h"
#include <stdlib.h>

#define CONFIRM_FORMAT      "请按C键确认"
#define PROMPT_DISK_MESSAGE "格式化分区"
#define PROMPT_DISK_SIZE    "分区大小: "
#define PROMPT_FAIL         "失败"
#define PROMPT_ERR_MESSAGE  "错误信息: "
#define PROMPT_SUCCESS      "成功!"
#define PROMPT_PROGRESS     "正在格式化..."

#define PROMPT_FAIL_BUSY    "设备忙!" 

// #define PROMPT_DISK_DEVNAME "设备名称: "
// #define PROMPT_DISK_MOUTDIR "挂载目录: "

namespace IBox {

DiskFormatView::DiskFormatView(int x, int y, int w, int h)
    : mShowWhat(-1), mFontSize(20), mErrorFlg(-1), mTotalSize("")
    , mResultMessage(""), mPartName(""), mMountDir(""), mBarHeight(25)
{
    setLoc(x, y);
    setSize(w, h);
}

DiskFormatView::~DiskFormatView()
{

}

DiskFormatView& 
DiskFormatView::setProgress(int progress)
{
    mProgress = progress;
    if (mShowWhat != e_FormatProgress)
        mShowWhat = e_FormatProgress;

    return *this;
}

DiskFormatView& 
DiskFormatView::promptDiskMessage(std::string partname, std::string mountdir, unsigned long long size)
{
    return promptDiskMessage(partname.c_str(), mountdir.c_str(), size);
}

DiskFormatView& 
DiskFormatView::promptDiskMessage(const char* partname, const char* mountdir, unsigned long long size)
{
    if (mShowWhat != e_PromptDiskMessage) {
        mPartName = partname ? partname : "";
        mMountDir = mountdir ? mountdir : "";
        mShowWhat = e_PromptDiskMessage;
    }
    char buff[64] = { 0 };
    snprintf(buff, 63, "%lluMB", size/1024);
    mTotalSize = buff;
    return *this;
}

DiskFormatView& 
DiskFormatView::promptResultMessage(int err, const char* result)
{
    if (mShowWhat != e_PromptResultMessage) {
        mErrorFlg = err;
        mResultMessage = result ? result : "";
        mShowWhat = e_PromptResultMessage;
    }
    return *this;
}

void 
DiskFormatView::onDraw(Canvas* canvas)
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
        cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);        

        if (mShowWhat == e_PromptDiskMessage) {
            cairo_translate (cr, 10, 15);
            cairo_set_source_rgb (cr, 1, 1, 1);        

            tmpstr = PROMPT_DISK_MESSAGE;
            tmpstr.append(": ");
            tmpstr.append(mPartName);

            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, tmpstr.c_str());

            tmpstr = PROMPT_DISK_SIZE;
            tmpstr.append(mTotalSize);

            cairo_translate (cr, 0, -extents.y_bearing + 15);
            cairo_text_extents (cr, tmpstr.c_str(), &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, tmpstr.c_str());

            cairo_translate (cr, 0, -extents.y_bearing + 25);
            cairo_set_source_rgb (cr, 1, 1, 0);
            cairo_text_extents (cr, CONFIRM_FORMAT, &extents);
            cairo_move_to (cr, 5, -extents.y_bearing);
            cairo_show_text (cr, CONFIRM_FORMAT);
        } else if (mShowWhat == e_PromptResultMessage) {
            if (mErrorFlg < 0) {
                cairo_translate (cr, 10, 45);
                cairo_set_source_rgb (cr, 1, 1, 1);        

                tmpstr = PROMPT_DISK_MESSAGE;
                tmpstr.append(mPartName);
                tmpstr.append(PROMPT_FAIL);

                cairo_text_extents (cr, tmpstr.c_str(), &extents);
                cairo_move_to (cr, 5, -extents.y_bearing);
                cairo_show_text (cr, tmpstr.c_str());

                tmpstr = PROMPT_ERR_MESSAGE;
                switch (mErrorFlg) {
                case FORMAT_IS_BUSY:
                case FORMAT_DISK_NOTFREE:
                case FORMAT_UNKNOW_PARTITION:
                case FORMAT_DEVICE_NOTFOUND:
                case FORMAT_PARTITION_NOTFOUND:
                case FORMAT_UNMOUNT_ERR:
                case FORMAT_EXECUTE_ERR:
                case FORMAT_MOUNT_ERR:
                    mResultMessage = PROMPT_FAIL_BUSY;
                    break;
                }

                tmpstr.append(mResultMessage);

                cairo_set_source_rgb (cr, 1, 1, 0);
                cairo_translate (cr, 0, -extents.y_bearing + 15);
                cairo_text_extents (cr, tmpstr.c_str(), &extents);
                cairo_move_to (cr, 5, -extents.y_bearing);
                cairo_show_text (cr, tmpstr.c_str());
            } else {
                cairo_translate (cr, 10, 45);
                cairo_set_source_rgb (cr, 1, 1, 1);        

                tmpstr = PROMPT_DISK_MESSAGE;
                tmpstr.append(mPartName);
                tmpstr.append(PROMPT_SUCCESS);

                cairo_text_extents (cr, tmpstr.c_str(), &extents);
                cairo_move_to (cr, 5, -extents.y_bearing);
                cairo_show_text (cr, tmpstr.c_str());
            }
        } else if (mShowWhat == e_FormatProgress) {
            x = 15.0;
            y = height() - mBarHeight - 20;

            cairo_set_source_rgb (cr, 1, 1, 1);        
            cairo_text_extents (cr, PROMPT_PROGRESS, &extents);
            cairo_move_to (cr, x + 5, -extents.y_bearing + 30);
            cairo_show_text (cr, PROMPT_PROGRESS);

            cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);                  
            cairo_rectangle (cr, x, y, (320-2*x), mBarHeight);
            cairo_fill (cr);   

            cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);                  
            cairo_rectangle (cr, x, y + 1.5, (320-2*x) * mProgress / 100 , mBarHeight - 3);
            cairo_fill (cr);  
        }
    }
    cairo_restore(cr);
}

void
DiskFormatView::onHandleInval(const Rect& r)
{
}

}
