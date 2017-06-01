#ifndef __DiskCopyVIEW__H_
#define __DiskCopyVIEW__H_

#include "View.h"
#include <string>

namespace IBox {

class ImageView;
class TextView;
class DiskCopyView : public View {
public:
    DiskCopyView(int x, int y, int w, int h);
    ~DiskCopyView();

    enum {
        e_DiskCopyPromptUName = 0,
        e_DiskCopyPromptScanFinish = 1,
        e_DiskCopyPromptSpaceNotEnough = 2,
        e_DiskCopyPromptCopyProgress = 3, 
        e_DiskCopyPromptPullout = 4,    
        e_DiskCopyPromptScanError = 5,    
        e_DiskCopyPromptScanProgress = 6,
    };

    DiskCopyView& setCopyProgress(int);
    DiskCopyView& setScanProgress(int);
    DiskCopyView& promptUNameMessage(std::string UName, float total, float avail);
    DiskCopyView& promptScanMessage();
    DiskCopyView& promptSpaceNotEnoughMessage();
    DiskCopyView& promptUPullOut();
    DiskCopyView& promptDiskScanError();
protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);
private:
    ImageView* mImage;
    ImageView* mImageOK;
    int mShowWhat;
    int mFontSize;
    int mBarHeight;
    int mCopyProgress;
    int mScanProgress;
    std::string mUName;
    std::string mTotalSize;  //32GB
    std::string mAvailSize;
    float mTotal;      //32
    float mAvail;
 //   int mSpace;
};

}
#endif
