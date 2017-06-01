#include "DateWidget.h"

namespace IBox {

static const char* gDateWeeks[7] = { "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

DateWidget::DateWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mMonth(32)
{
    mDateTxt = _CreateTextView(5, 5, 150, 45);
    mDateTxt->setFontSize(20);
    mDateTxt->setBGColor(0, 0, 0, 0);
    mDateTxt->setFontColor(0x99, 0x99, 0x99, 0xff);
    mDateTxt->setAlignStyle(TextView::ALIGN_CENTER_BOTTOM);
    mDateTxt->setText("01月01日 星期一").inval(0);
}

DateWidget::~DateWidget()
{
    if (mDateTxt) {
        mDateTxt->detachFromParent();
        delete mDateTxt;
    }
}

int 
DateWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mDateTxt);
    }
}

bool 
DateWidget::isSameDate(const SysTime::DateTime& dt)
{
    return (dt.mMonth == mMonth 
        && dt.mDay == mDay
        && dt.mDayOfWeek == mDayOfWeek);
}

void 
DateWidget::syncDate(const SysTime::DateTime& dt)
{
    mMonth = dt.mMonth;
    mDay = dt.mDay;
    mDayOfWeek = dt.mDayOfWeek;
}

int 
DateWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (isSameDate(dt))
        return 0;
    syncDate(dt);

    char txt[64] = { 0 };
    sprintf(txt, "%02d月%02d日 %s", mMonth, mDay, gDateWeeks[mDayOfWeek]);
    mDateTxt->setText(txt).inval(0);
    mDateTxt->setVisibleP(true);

    return 1;
}

}
