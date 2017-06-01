#ifndef __SlideImage__H_
#define __SlideImage__H_

#include "ImageView.h"

#include <string>

namespace IBox {

typedef enum {
    e_DISPLAY_FULLSCREEN   = 0,
    e_DISPLAY_OPTIMALRATIO = 1,
}DISPLAY_MODE_e;

typedef enum {
    e_EFFECT_NONE = 0,
    e_EFFECT_UNCOVERRIGHT = 1,
    e_EFFECT_FADEOUTSMOOTHLY = 2,
}EFFECT_TYPE_e;

class SlideEffect;
class SlideImage : public ImageView {
public:
    SlideImage(int x, int y, int w, int h);
    ~SlideImage();

    int startEffect(int);

    void setDisplayMode(int mode);   

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);

private:
    int mDisplayMode;
    SlideEffect* mEffect;
};

}

#endif
