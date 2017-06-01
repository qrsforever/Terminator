#ifndef __FanStatusWidget__H_
#define __FanStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class FanStatusWidget : public Widget {
public:
    FanStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~FanStatusWidget();
private:
    ImageView* mFanImg;
    unsigned int mNextRefreshTime;
    int mRefSpeed;
    int mCurrentState;
};

}

#endif
