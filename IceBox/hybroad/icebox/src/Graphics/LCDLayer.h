#ifndef _LCDLayer_H_
#define _LCDLayer_H_

#ifdef __cplusplus

#include "Canvas.h"

namespace IBox {

class Canvas;
class LCDLayer {
private:
    LCDLayer();
    ~LCDLayer();

public:
    static LCDLayer& self();
    void show();
    void hide();

    int mHandle;
    int mWidth;
    int mHeight;
    Canvas* mCanvas;
};

} // namespace IBox

#endif // __cplusplus

#endif // _LCDLayer_H_
