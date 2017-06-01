#ifndef __PICTUREDISPLAY__H_
#define __PICTUREDISPLAY__H_

#include "TVScreen.h"
#include "ImageView.h"

namespace IBox {

class SlideTransition;
class DisplayScreen : public TVScreen {

public:
    DisplayScreen(Canvas* c, int w, int h, int z);
    ~DisplayScreen();

    void setBackgroud(const char* imgpath);

    SlideTransition& slide();

protected:
    virtual void onDraw(Canvas*);

private:
    ImageView* mBGImage;
    SlideTransition* mSlideTrans;
};

}

#endif
