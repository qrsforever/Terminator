#ifndef __SDCardStatusWidget__H_
#define __SDCardStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class SDCardStatusWidget : public Widget {
public:
    SDCardStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int clocktime, const SysTime::DateTime&, int);

    ~SDCardStatusWidget();
private:
    ImageView* mSDCardImg;
    int mCurrentState;
    unsigned int mNextRefreshTime;
};

}
#endif
