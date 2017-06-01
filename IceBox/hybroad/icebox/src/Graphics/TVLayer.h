#ifndef _TVLayer_H_
#define _TVLayer_H_

#ifdef __cplusplus

#include "Canvas.h"

namespace IBox {

class Canvas;
class TVLayer {
private:
    TVLayer();
    ~TVLayer();

public:
    static TVLayer& self();
    void show();
    void hide();

    int mHandle;
    int mWidth;
    int mHeight;
    Canvas* mCanvas;
};

} // namespace IBox

#endif // __cplusplus

#endif // _TVLayer_H_
