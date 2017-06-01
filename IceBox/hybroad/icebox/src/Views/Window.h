#ifndef _Window_H_
#define _Window_H_

#include "View.h"
#include "Region.h"

#ifdef __cplusplus

namespace IBox {

class ColorAttr {
public:
    ColorAttr():r(0),g(0),b(0),a(0){}
    ColorAttr(int _r, int _g, int _b, int _a) { r = _r; g = _g; b = _b; a = _a; }
    ColorAttr(const ColorAttr& o){ r = o.r; g = o.g; b = o.b; a = o.a; }
    void set(int _r, int _g, int _b, int _a) { r = _r; g = _g; b = _b; a = _a; }
    int r, g, b, a;
};

class Window : public View {
public:
            Window();
    virtual ~Window();

    void    resize(int pWidth, int pHeight);

    bool    isDirty() /*const*/ { return !fDirtyRgn.isEmpty(); }
    bool    update(Rect* updateArea, Canvas* canvas);

protected:
    // called if part of our bitmap is invalidated
    virtual void onHandleInval(const Rect&);

    // overrides from View
    virtual bool handleInval(const Rect&);

//private:
    //SkBitmap::Config    fConfig;
    //SkBitmap    fBitmap;
    Region    fDirtyRgn;

    bool fWaitingOnInval;

    typedef View INHERITED;
};

} // namespace IBox

#endif // __cplusplus

#endif // _Window_H_
