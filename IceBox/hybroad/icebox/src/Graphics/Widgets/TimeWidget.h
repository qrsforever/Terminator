#ifndef __TimeWidget__H_
#define __TimeWidget__H_ 

#include "Widget.h"

namespace IBox { 

class TimeWidget : public Widget {
public:
    TimeWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~TimeWidget();
    bool isSameTime(const SysTime::DateTime&);
    void syncTime(const SysTime::DateTime&);
private:
    int mHour;
    int mMinute;
    TextView* mTimeTxt;
    ImageView* mLineImg;
};

}
#endif
