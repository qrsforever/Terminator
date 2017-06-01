#include "GraphicsAssertions.h"
#include "LCDScreen.h"

#include "Canvas.h"

#include "Message.h"
#include "MessageTypes.h"
#include "NativeHandler.h"

namespace IBox {

LCDScreen::LCDScreen(Canvas* canvas, int w, int h, int zorder)
	: mCanvas(canvas), mZorder(zorder)
{
    setLoc(0, 0);
    setSize(w, h);
}

LCDScreen::~LCDScreen()
{
}

bool
LCDScreen::repaint(Rect* rect, Canvas* canvas, bool copy)
{
    return update(rect, mCanvas);
}

void
LCDScreen::onDraw(Canvas* canvas)
{
    cairo_t* cr = canvas->mCairo;

    cairo_save(cr);
    cairo_rectangle(cr, 0, 0, width(), height());
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_fill(cr);
    cairo_restore(cr);
}

void
LCDScreen::onHandleInval(const Rect& r)
{
    Message *msg = defNativeHandler().obtainMessage(MessageType_Repaint, 1, 0, this);
    defNativeHandler().sendMessage(msg);
}

} // namespace IBox
