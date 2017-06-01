#ifndef __UncoverRight__H_
#define __UncoverRight__H_

#include "SlideEffect.h"

namespace IBox {

class UncoverRight : public SlideEffect {
public:
    UncoverRight();
    ~UncoverRight();

    virtual void execute(SlideImage* slide);
    virtual double beforeApply(cairo_t* cr);
    virtual double afterApply(cairo_t* cr);
private:
    int mActionTime;
    int mStepCnt;
    int mOriginX;
};

}

#endif
