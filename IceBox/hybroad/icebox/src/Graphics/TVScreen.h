#ifndef _TVScreen_H_
#define _TVScreen_H_

#include "Window.h"

#define SCREEN_MAX_WIDTH  320
#define SCREEN_MAX_HEIGHT 240

#ifdef __cplusplus

namespace IBox {

class Canvas;
class TVScreen : public Window {
public:
    TVScreen(Canvas*, int w, int h, int zorder);
    ~TVScreen();
    virtual bool repaint(Canvas* canvas = 0, bool copy = 0);

    int Zorder() { return mZorder; }
protected:
    /** Override this to draw inside the view. Be sure to call the inherited version too */
    virtual void onDraw(Canvas*);

    virtual void onHandleInval(const Rect&);

	Canvas* mCanvas;
private:
    int mZorder;
};

} // namespace IBox

#endif // __cplusplus

#endif // _TVScreen_H_
