#include "GraphicsAssertions.h"
#include "TextView.h"
#include "Canvas.h"

#define BORDER_SIZE 1.0

namespace IBox {

TextView::TextView(int x, int y, int w, int h)
    : mText(""), mFontSize(12)
{
    setLoc(x, y);
    setSize(w, h);
}

TextView::~TextView()
{

}

void
TextView::setBGColor(int r, int g, int b, int a)
{
    mBGColor = ColorAttr(r, g, b, a);
}

void
TextView::setFontColor(int r, int g, int b, int a)
{
    mFontColor = ColorAttr(r, g, b, a);
}

TextView&
TextView::setText(const char* text)
{
    if (text)
        mText = text;
    return *this;
}

void 
TextView::onDraw(Canvas* canvas)
{
    cairo_text_extents_t extents;
    double x = 0, y = 0;

    cairo_t* cr = canvas->mCairo;

    cairo_save(cr); 
    {
        // mBGColor.r = 255; mBGColor.a = 255; //FOR DEBUG
        cairo_set_source_rgba (cr, (double)mBGColor.r/255, (double)mBGColor.g/255, (double)mBGColor.b/255, (double)mBGColor.a/255);
        cairo_paint (cr);

        cairo_set_source_rgba (cr, (double)mFontColor.r/255, (double)mFontColor.g/255, (double)mFontColor.b/255, (double)mFontColor.a/255);
        cairo_set_font_size(cr, mFontSize);
        cairo_text_extents(cr, mText.c_str(), &extents);

        switch (mAlign) {
        case ALIGN_LEFT_TOP:
            x = BORDER_SIZE;
            y = BORDER_SIZE - extents.y_bearing;
            break;
        case ALIGN_LEFT_CENTER:
            x = BORDER_SIZE;
            y = (height() >> 1) - (extents.height/2 + extents.y_bearing);
            break;
        case ALIGN_LEFT_BOTTOM:
            x = BORDER_SIZE;
            y = height() - (extents.height + extents.y_bearing) - BORDER_SIZE;
            break;
        case ALIGN_CENTER_BOTTOM:
            x = (width() >> 1) - (extents.width/2 + extents.x_bearing);
            y = height() - (extents.height + extents.y_bearing) - BORDER_SIZE;
            break;
        case ALIGN_CENTER:
        default:
            x = (width() >> 1) - (extents.width/2 + extents.x_bearing);
            y = (height() >> 1) - (extents.height/2 + extents.y_bearing);
            break;
        }
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, mText.c_str());
    }
    cairo_restore(cr);

    // GRAPHICS_LOG("%s <%d %d %d %d>\n", mText.c_str(), locX(), locY(), width(), height());
}

void
TextView::onHandleInval(const Rect& r)
{
}

}
