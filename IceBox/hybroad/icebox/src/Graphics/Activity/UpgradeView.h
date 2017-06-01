#ifndef __UpgradeVIEW__H_
#define __UpgradeVIEW__H_ 

#include "View.h"
#include <string>

namespace IBox {

class ImageView;
class TextView;
class UpgradeView : public View {
public:
    UpgradeView(int x, int y, int w, int h);
    ~UpgradeView();

    UpgradeView& setDownloadProgress(int);
    UpgradeView& setBurningProgress(int);

    enum {
        e_UpgradePromptVersion = 0,
        e_UpgradeDProgress = 1,
        e_UpgradeBProgress = 2,
        e_UpgradePromptSuccess = 3,
    };

    UpgradeView& promptSuccessMessage(const char* success = 0);
    UpgradeView& promptVersionMessage(const char* versionNumber, bool localUpgrade = false);

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);
private:
    int mShowWhat;
    bool mLocalUpgrade;
    int mFontSize;
    int mBarHeight;
    double mNowSize;
    double mTotalSize;
    int mProgress;
    std::string mMesage;
    std::string mVersionNumber;
    ImageView* mImage;
};

}


#endif
