#ifndef __ImageView__H_
#define __ImageView__H_

#include "LCDScreen.h"

#include <stdint.h>
#include <string>
#include "Canvas.h"

namespace IBox {

class Mutex;
class Picture;
class ImageView : public View {
public:
    ImageView(int x, int y, int w, int h);
    ~ImageView();

    void setImageAlpha(double alpha) { mAlpha = alpha; }
    void setBGColor(int r, int g, int b, int a = 0);
    ImageView& setSource(const char* path);
    void delSource();
    
    virtual int typeID() { return 500; }

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);

protected:
    Mutex* mPicMutex;
    Picture* mPic;
    ColorAttr mBGColor;
    double mAlpha;
    cairo_surface_t* mSurface;
};

}

#endif
