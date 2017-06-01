#include "GraphicsAssertions.h"
#include "ImageView.h"
#include "Canvas.h"
#include "Mutex.h"
#include "Picture.h"

namespace IBox {

ImageView::ImageView(int x, int y, int w, int h) 
    : mPic(0), mSurface(0), mAlpha(1.0)
{
    setLoc(x, y);
    setSize(w, h);
    mBGColor.set(0, 0, 0, 0);
    mPicMutex = new Mutex("ImageView");
}

ImageView::~ImageView()
{
    delSource();
    delete mPicMutex;
}

void 
ImageView::delSource()
{
    if (mPic) {
        mPicMutex->lock();
        if (mSurface)
            cairo_surface_destroy(mSurface);
        delete mPic;
        mPicMutex->unlock();
    }
    mSurface = 0;
    mPic = 0;
}

ImageView&
ImageView::setSource(const char* path)
{
    delSource();

    mPicMutex->lock();
    mPic = Picture::create(path);
    if (mPic)
        mPic->load();
    mPicMutex->unlock();
    return *this;
}

void
ImageView::setBGColor(int r, int g, int b, int a)
{
    mBGColor = ColorAttr(r, g, b, a);
}

void
ImageView::onDraw(Canvas* canvas)
{
    mPicMutex->lock();
    if (!mPic) {
        mPicMutex->unlock();
        return;
    }
    cairo_t* cr = canvas->mCairo;
    cairo_save (cr);   
    {
        // mBGColor.g = 255; mBGColor.a = 255; //FOR DEBUG
        cairo_set_source_rgba (cr, (double)mBGColor.r/255, (double)mBGColor.g/255, (double)mBGColor.b/255, (double)mBGColor.a/255);
        cairo_paint (cr);

        //TODO LCD display only use cairo_image_surface_create_from_png now, fixme please.
        int imagew = 0, imageh = 0;
        if (!mSurface) {
            if (mPic->pictype() == Picture::PIC_PNG) {
                //PNG
                mSurface = cairo_image_surface_create_from_png(mPic->file());
            } else {
                //JPG, BMP, GIF
                mSurface = cairo_image_surface_create_for_data(mPic->bitmap(), CAIRO_FORMAT_ARGB32, mPic->width(), mPic->height(), mPic->stride());
                // LCD layer only support CAIRO_FORMAT_RGB16_565, using CAIRO_FORMAT_ARGB32 or CAIRO_FORMAT_RGB24 will fail.
            }
        }
        if (mSurface) {
            imagew = cairo_image_surface_get_width(mSurface);
            imageh = cairo_image_surface_get_height(mSurface);
            cairo_scale(cr, (double)width()/imagew, (double)height()/imageh);
            cairo_set_source_surface (cr, mSurface, 0, 0);
            cairo_paint_with_alpha(cr, mAlpha);
        }
    }
    cairo_restore (cr);                                            
    // GRAPHICS_LOG("%s <%d %d %d %d>\n", mPic->file(), locX(), locY(), width(), height());
    mPicMutex->unlock();
}

void
ImageView::onHandleInval(const Rect& r)
{
}

}
