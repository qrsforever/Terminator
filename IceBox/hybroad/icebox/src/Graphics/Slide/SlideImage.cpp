#include "SlideImage.h"
#include "UncoverRight.h"
#include "FadeoutSmoothly.h"
#include "Picture.h"
#include "Canvas.h"
#include "Mutex.h"

#include <math.h>

namespace IBox {

SlideImage::SlideImage(int x, int y, int w, int h)
    : ImageView(x, y, w, h), mEffect(0)
    , mDisplayMode(e_DISPLAY_OPTIMALRATIO) 
{ 

}

SlideImage::~SlideImage()
{
    if (mEffect)
        delete mEffect;
    mEffect = 0;
}

void SlideImage::setDisplayMode(int mode) 
{
    mDisplayMode = mode; 
}

int 
SlideImage::startEffect(int type)
{
    if (mEffect)
        delete mEffect;
    mEffect = 0;

    switch (type) {
    case e_EFFECT_UNCOVERRIGHT:
        mEffect = new UncoverRight();
        break;
    case e_EFFECT_FADEOUTSMOOTHLY:
        mEffect = new FadeoutSmoothly();
        break;
    }
    if (mEffect)
        mEffect->execute(this);
    delete mEffect;
    mEffect = 0;
}

void 
SlideImage::onDraw(Canvas* canvas)
{
    mPicMutex->lock();
    if (!mPic || !mPic->isValid()) {
        mPicMutex->unlock();
        return;
    }
    cairo_t* cr = canvas->mCairo;
    cairo_save (cr);   
    {
        // mBGColor.g = 255; mBGColor.a = 255; //FOR DEBUG
        cairo_set_source_rgba (cr, (double)mBGColor.r/255, (double)mBGColor.g/255, (double)mBGColor.b/255, (double)mBGColor.a/255);
        cairo_paint(cr);

        double alpha = 1.0;
        if (mEffect)
            alpha = mEffect->beforeApply(cr);

        if (!mSurface)
            mSurface = cairo_image_surface_create_for_data(mPic->bitmap(), CAIRO_FORMAT_ARGB32, mPic->width(), mPic->height(), mPic->stride());

        int imagew = cairo_image_surface_get_width(mSurface);
        int imageh = cairo_image_surface_get_height(mSurface);

        double sw = (double)width()/imagew;
        double sh = (double)height()/imageh;
        double scale = sw > sh ? sh : sw;
        if (e_DISPLAY_OPTIMALRATIO == mDisplayMode) {
            if (scale < 1.0) {
                if (sw > sh)
                    cairo_translate(cr, (width()-imagew*scale)/2 , 0);
                else
                    cairo_translate(cr, 0, (height()-imageh*scale)/2);
                cairo_scale(cr, scale, scale);
            } else {
                cairo_translate(cr, (width()-imagew)/2, (height()-imageh)/2);
            }
        } else if (e_DISPLAY_FULLSCREEN == mDisplayMode) {
            cairo_scale(cr, sw, sh);
        }

        if (mEffect)
            alpha = mEffect->afterApply(cr);

        cairo_set_source_surface(cr, mSurface, 0, 0);
        cairo_paint_with_alpha(cr, alpha);
    }
    cairo_restore (cr);                                            
    mPicMutex->unlock();
}

void
SlideImage::onHandleInval(const Rect& r)
{
}

}
