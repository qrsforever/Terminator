#ifndef __USBStatusWidget__H_
#define __USBStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class USBStatusWidget : public Widget {
public:
    USBStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~USBStatusWidget();
private:
    ImageView* mUSBImg;
    int mCurrentState;
    unsigned int mNextRefreshTime;
};

}

#endif
