#include "GraphicsAssertions.h"
#include "UncoverRight.h"
#include "SlideImage.h"

#include <math.h>
#include <unistd.h>

namespace IBox {

UncoverRight::UncoverRight() : mStepCnt(80),mActionTime(2000) 
{
}   

UncoverRight::~UncoverRight()
{
}

void 
UncoverRight::execute(SlideImage* slide)
{
    int width = slide->width();
    int step = width / mStepCnt;
    int sleep = (mActionTime / mStepCnt) * 1000;
    mOriginX = step - width;
    GRAPHICS_LOG("run slide step[%d] sleep[%d]\n",step,sleep);
    for (int i = 0;i < mStepCnt; i++) {
        mOriginX += step;
        slide->inval(0);
        usleep(sleep);
    }
    if (mOriginX) {
        mOriginX = 0;
        slide->inval(0);
    }
}

double
UncoverRight::afterApply(cairo_t* cr)
{
    cairo_translate(cr,mOriginX,0);
    return 1.0;
}

double 
UncoverRight::beforeApply(cairo_t* cr)
{
    return 1.0;
}
}
