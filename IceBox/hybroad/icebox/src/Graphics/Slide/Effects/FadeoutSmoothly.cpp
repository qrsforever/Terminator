#include "GraphicsAssertions.h"
#include "FadeoutSmoothly.h"
#include "SlideImage.h"

#include <math.h>
#include <unistd.h>

namespace IBox {
    
FadeoutSmoothly::FadeoutSmoothly() : mStepCnt(100), mActionTime(2000 /*ms*/), mImage(0)
{
}

FadeoutSmoothly::~FadeoutSmoothly()
{ 
}

void 
FadeoutSmoothly::execute(SlideImage* slide)
{
    mImage = slide;
    mAngle = 0.0;
    mScaleX = 0.0;
    mScaleY = 0.0;
    mAlpha = 0.0;
    mOriginX = mImage->width()/2;
    mOriginY = mImage->height()/2;

    double scale = 1.0/mStepCnt;
    double alpha = 2.0/mStepCnt;
    double angle = M_PI*2/mStepCnt;
    int sleep = (mActionTime / mStepCnt) * 1000; /*us*/
    GRAPHICS_LOG("run slide step[%d] sleep[%d]\n", mStepCnt, sleep); 
    for (int i = 0; i < mStepCnt; i++) {
        mAngle += angle;
        mScaleX += scale;
        mScaleY += scale;
        mAlpha += alpha;
        slide->inval(0);
        usleep(sleep);
    }

    mAngle = 0.0;
    mScaleX = 1.0;
    mScaleY = 1.0;
    mAlpha = 1.0;
    slide->inval(0);
}

double
FadeoutSmoothly::afterApply(cairo_t* cr)
{
    cairo_translate(cr, mOriginX-mOriginX*mScaleX, mOriginY-mOriginY*mScaleY);
    cairo_scale(cr, mScaleX, mScaleY);
    //cairo_rotate(cr, mAngle);
    return mAlpha;
}
double
FadeoutSmoothly::beforeApply(cairo_t* cr)
{
    if (mImage) {
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
        cairo_rectangle(cr, 0.0, 0.0, mImage->width(), mImage->height());
        cairo_fill(cr);
    }
    return 1.0;
}

}
