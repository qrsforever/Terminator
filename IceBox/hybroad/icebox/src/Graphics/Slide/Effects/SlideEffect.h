#ifndef __SlideEffects__H_
#define __SlideEffects__H_

#include "cairo.h"

namespace IBox {
    
class SlideImage;
class SlideEffect {
public:
    SlideEffect();
    virtual ~SlideEffect();

    virtual void execute(SlideImage* slide) = 0;
    virtual double beforeApply(cairo_t* cr) = 0;
    virtual double afterApply(cairo_t* cr) = 0;
};

}


#endif
