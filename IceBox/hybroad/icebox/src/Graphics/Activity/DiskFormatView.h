#ifndef __DiskFormatVIEW__H_
#define __DiskFormatVIEW__H_ 

#include "View.h"
#include <string>

#define FORMAT_SUCCESS        0
#define FORMAT_IS_BUSY       -10
#define FORMAT_DISK_NOTFREE  -11

#define FORMAT_UNKNOW_PARTITION    -100
#define FORMAT_DEVICE_NOTFOUND     -200 
#define FORMAT_PARTITION_NOTFOUND  -300
#define FORMAT_UNMOUNT_ERR         -400
#define FORMAT_EXECUTE_ERR         -500
#define FORMAT_MOUNT_ERR           -600

namespace IBox {

class DiskFormatView : public View {
public:
    DiskFormatView(int x, int y, int w, int h);
    ~DiskFormatView();

    DiskFormatView& setProgress(int progress);

    enum {
        e_PromptDiskMessage = 0,
        e_FormatProgress = 1,
        e_PromptResultMessage = 2,
    };

    DiskFormatView& promptDiskMessage(std::string partname, std::string mountdir, unsigned long long size = 0);
    DiskFormatView& promptDiskMessage(const char* partname, const char* mountdir, unsigned long long size = 0);
    DiskFormatView& promptResultMessage(int err, const char* result = 0);

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);
private:
    int mShowWhat;
    int mFontSize;
    int mProgress;
    int mErrorFlg;
    int mBarHeight;
    std::string mTotalSize;
    std::string mResultMessage;
    std::string mPartName;
    std::string mMountDir;
};

}


#endif
