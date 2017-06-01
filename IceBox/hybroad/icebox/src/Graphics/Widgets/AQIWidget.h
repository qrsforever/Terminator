#ifndef __AQIWidget__H_
#define __AQIWidget__H_ 

#include "Widget.h"

namespace IBox { 

class AQIWidget : public Widget {
public:
    AQIWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~AQIWidget();
private:
    int mAQI;
    ImageView* mAQIImg;
    TextView* mAQITxt;
};

}

#endif
