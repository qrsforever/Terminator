#include "GraphicsAssertions.h"
#include "AuthCodeView.h"
#include "Canvas.h"
#include <stdlib.h>

#define ID_CODE_LABEL "验证码:"

namespace IBox {

AuthCodeView::AuthCodeView(int x, int y, int w, int h)
    : mLabelFontSize(28),  mCodeFontSize(50)
{
    setLoc(x, y);
    setSize(w, h);
}

AuthCodeView::~AuthCodeView()
{

}

AuthCodeView& 
AuthCodeView::setCodeString(const char* string)
{
    mCodeString = "";
    if (string)
        mCodeString = string;
    return *this;
}

AuthCodeView& 
AuthCodeView::setCodeNumber(int code)
{
    unsigned int number = 0;
    if (code < 0)
        number = -1 * code;
    else 
        number = code;

    mCodeString = "";
    char buff[16] = { 0 };
    snprintf(buff, 15, "%6d", number);
    mCodeString = buff;
    return *this;
}

std::string
AuthCodeView::getCodeString()
{
    if (mCodeString.empty())
        return 0;
    return mCodeString;
}

int 
AuthCodeView::getCodeNumber()
{
    if (mCodeString.empty())
        return -1;
    return atoi(mCodeString.c_str());
}

void 
AuthCodeView::onDraw(Canvas* canvas)
{
    cairo_text_extents_t extents;
    double x = 0, y = 0;

    cairo_t* cr = canvas->mCairo;

    cairo_save(cr); 
    {
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);                  
        cairo_paint(cr);
        cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);        

        cairo_set_font_size(cr, mLabelFontSize);
        cairo_text_extents(cr, ID_CODE_LABEL, &extents);
        x = 10.0;
        y = 40.0;
        cairo_move_to(cr, x, y);
        cairo_show_text (cr, ID_CODE_LABEL);

        cairo_set_font_size(cr, mCodeFontSize);
        cairo_text_extents(cr, mCodeString.c_str(), &extents);
        x = (width() / 2) - (extents.width/2 + extents.x_bearing);
        y = (height() * 3 / 4 ) - (extents.height/2 + extents.y_bearing);
        cairo_move_to(cr, x, y);
        cairo_show_text (cr, mCodeString.c_str());
    }
    cairo_restore(cr);
}

void
AuthCodeView::onHandleInval(const Rect& r)
{
}

}
