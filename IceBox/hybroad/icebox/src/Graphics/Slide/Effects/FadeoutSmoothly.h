#ifndef __FadeoutSmoothly__H_
#define __FadeoutSmoothly__H_

#include "SlideEffect.h"

namespace IBox {

class FadeoutSmoothly : public SlideEffect {
public:
    FadeoutSmoothly();
    ~FadeoutSmoothly();

    virtual void execute(SlideImage* slide);
    virtual double beforeApply(cairo_t* cr);
    virtual double afterApply(cairo_t* cr);
private:
    int mActionTime;
    int mStepCnt;
    double mAngle;
    double mScaleX;
    double mScaleY;
    double mAlpha;
    double mOriginX;
    double mOriginY;
    SlideImage* mImage;
};

}

#endif
