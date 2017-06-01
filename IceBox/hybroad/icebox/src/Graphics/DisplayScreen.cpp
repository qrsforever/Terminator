#include "GraphicsAssertions.h"
#include "DisplayScreen.h"
#include "SlideTransition.h"

namespace IBox {

DisplayScreen::DisplayScreen(Canvas* canvas, int w, int h, int z)
    : TVScreen(canvas, w, h, z), mBGImage(0), mSlideTrans(0)
{
}

DisplayScreen::~DisplayScreen()
{
    if (mSlideTrans)
        delete mSlideTrans;
}

SlideTransition& 
DisplayScreen::slide()
{
    if (!mSlideTrans)
        mSlideTrans = new SlideTransition(*this);
    return *mSlideTrans;
}

void 
DisplayScreen::setBackgroud(const char* imgpath)
{
    if (!mBGImage) {
        mBGImage = new ImageView(locX(), locY(), width(), height());
        attachChildToBack(mBGImage);
    }

    mBGImage->setVisibleP(false);

    if (imgpath) {
        mBGImage->setVisibleP(true);
        mBGImage->setSource(imgpath).inval(0);
    }
}

void
DisplayScreen::onDraw(Canvas* canvas)
{
    cairo_t* cr = canvas->mCairo;

    cairo_save(cr);
    {
        cairo_rectangle(cr, 0, 0, width(), height());
        cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
        cairo_fill(cr);
    }
    cairo_restore(cr);
}

}
