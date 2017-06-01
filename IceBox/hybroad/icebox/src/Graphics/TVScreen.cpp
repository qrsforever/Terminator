#include "GraphicsAssertions.h"
#include "TVScreen.h"

#include "Canvas.h"

#include "Message.h"
#include "MessageTypes.h"
#include "NativeHandler.h"

namespace IBox {

TVScreen::TVScreen(Canvas* canvas, int w, int h, int zorder)
	: mCanvas(canvas), mZorder(zorder)
{
    setLoc(0, 0);
    setSize(w, h);
}

TVScreen::~TVScreen()
{
}

bool
TVScreen::repaint(Canvas* canvas, bool copy)
{
    return update(NULL, mCanvas);
}

void
TVScreen::onDraw(Canvas* canvas)
{
    cairo_t* cr = canvas->mCairo;

    cairo_save(cr);
    cairo_rectangle(cr, 0, 0, width(), height());
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_fill(cr);
    cairo_restore(cr);
}

void
TVScreen::onHandleInval(const Rect& r)
{
    Message *msg = defNativeHandler().obtainMessage(MessageType_Repaint, 2, 0, this);
    defNativeHandler().sendMessage(msg);
}

} // namespace IBox
