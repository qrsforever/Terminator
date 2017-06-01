#ifndef _LCDScreen_H_
#define _LCDScreen_H_

#include "Rect.h"
#include "Window.h"

#define SCREEN_MAX_WIDTH  320
#define SCREEN_MAX_HEIGHT 240

#ifdef __cplusplus

namespace IBox {

class Canvas;
class LCDScreen : public Window {
public:
    LCDScreen(Canvas*, int w, int h, int zorder);
    ~LCDScreen();
    virtual bool repaint(Rect* rect = 0, Canvas* canvas = 0, bool copy = 0);

    // void hide( ) { setVisibleP(false); }
    // void show( ) { inval(NULL); setVisibleP(true); }

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

#endif // _LCDScreen_H_
