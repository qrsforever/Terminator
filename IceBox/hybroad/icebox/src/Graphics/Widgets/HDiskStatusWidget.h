#ifndef __HDiskStatusWidget__H_
#define __HDiskStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class HDiskStatusWidget : public Widget {
public:
    HDiskStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int clocktime, const SysTime::DateTime&, int);

    ~HDiskStatusWidget();
private:
    ImageView* mHDiskImg;
    int mCurrentState;
    unsigned int mNextRefreshTime;
};

}
#endif
