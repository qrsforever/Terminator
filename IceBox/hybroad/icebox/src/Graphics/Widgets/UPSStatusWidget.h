#ifndef __UPSStatusWidget__H_
#define __UPSStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class UPSStatusWidget : public Widget {
public:
    UPSStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    int setChargeStatus(int status) { mChargeStatus = status; }

    ~UPSStatusWidget();
private:
    ImageView* mUPSImg;
    unsigned int mNextRefreshTime;
    int mChargeStatus;
    int mCurrentState;
};

}
#endif
