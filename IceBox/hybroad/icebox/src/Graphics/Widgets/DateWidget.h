#ifndef __DateWidget__H_
#define __DateWidget__H_ 

#include "Widget.h"

namespace IBox { 

class DateWidget : public Widget {
public:
    DateWidget(int id, int x, int y, int w, int h);
    ~DateWidget();

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

private:
    bool isSameDate(const SysTime::DateTime&);
    void syncDate(const SysTime::DateTime&);
    int mMonth;
    int mDay;
    int mDayOfWeek;
    TextView* mDateTxt;
};

}
#endif
