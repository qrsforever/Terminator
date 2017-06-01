#include "GraphicsAssertions.h"
#include "Widget.h"

namespace IBox {

Widget::Widget(int id, int x, int y, int w, int h) 
    : mID(id), mX(x), mY(y), mW(w), mH(h)
    , mHandler(0), mRefreshFlg(0)
{

}

Widget::~Widget()
{
}

ImageView* 
Widget::_CreateImageView(int x, int y, int w, int h)
{
    //TODO
    if ((x + w) > (mX + mW)) {
        GRAPHICS_LOG_WARNING("(%d + %d) > (%d + %d)\n", x, w, mX, mW);
        return new ImageView(mX, mY, mW, mH);
    }
    if ((y + h) > (mY + mH)) {
        GRAPHICS_LOG_WARNING("(%d + %d) > (%d + %d)\n", y, h, mY, mH);
        return new ImageView(mX, mY, mW, mH);
    }
    return new ImageView(mX + x, mY + y, w, h);
}

TextView* 
Widget::_CreateTextView(int x, int y, int w, int h)
{
    //TODO
    if ((x + w) > (mX + mW)) {
        GRAPHICS_LOG_WARNING("(%d + %d) > (%d + %d)\n", x, w, mX, mW);
        return new TextView(mX, mY, mW, mH);
    }
    if ((y + h) > (mY + mH)) {
        GRAPHICS_LOG_WARNING("(%d + %d) > (%d + %d)\n", y, h, mY, mH);
        return new TextView(mX, mY, mW, mH);
    }
    return new TextView(mX + x, mY + y, w, h);
}

Widget::MixNullView::MixNullView(int x, int y, int w, int h)
{
    setLoc(x, y);
    setSize(w, h);
}

Widget::MixNullView::~MixNullView()
{
}

}
