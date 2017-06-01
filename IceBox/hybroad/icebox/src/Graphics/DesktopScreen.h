#ifndef __DESKSCREEN__H_
#define __DESKSCREEN__H_

#include "Widget.h"

#include "LCDScreen.h"
#include <map>

namespace IBox {

class ImageView;
class UpgradeView;
class AuthCodeView;
class EasysetupView;
class DiskFormatView;
class DiskCopyView;
class DesktopScreen : public LCDScreen {
public:
    DesktopScreen(Canvas* c, int w, int h, int z);
    ~DesktopScreen();

    void setBackgroud(const char* imgpath);

    void addWidget(Widget* widget);
    Widget* getWidget(int id);

    void showMainLayer();
    void hideMainLayer();
    UpgradeView* getUpgradeView() { return mUpgradeLayer; }

    void showUpgradeLayer();
    void hideUpgradeLayer();

    void showIdentifyCode(const char* code);
    int getIdentifyCode();
    void clearIdentifyCode();

    void showEasysetupLayer();
    EasysetupView* getEasysetupView();
    void clearEasysetupLayer();

    void showDiskFormatLayer();
    DiskFormatView* getDiskFormatView();
    void clearDiskFormatLayer();

    void showDiskCopyLayer();
    DiskCopyView* getDiskCopyView();
    void clearDiskCopyLayer();

    int refreshAllWidgets();

protected:
    virtual void onDraw(Canvas*);

private:
    std::map<int, Widget*> mWidgets;
    ImageView* mBGImage;
    UpgradeView* mUpgradeLayer;
    AuthCodeView* mAuthCodeLayer;
    EasysetupView* mEasysetupLayer;
    DiskFormatView* mDiskFormatLayer;
    DiskCopyView* mDiskCopyLayer;
    View* mMainLayer;
};

}

#endif
