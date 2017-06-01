#ifndef __WifiStatusWidget__H_
#define __WifiStatusWidget__H_ 

#include "Widget.h"

namespace IBox { 

class WifiStatusWidget : public Widget {
public:
    WifiStatusWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~WifiStatusWidget();
private:
    ImageView* mWifiImg;
};

}
#endif
