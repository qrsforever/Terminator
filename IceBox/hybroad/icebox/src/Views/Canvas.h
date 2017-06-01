#ifndef _Canvas_H_
#define _Canvas_H_

#include "Types.h"
#include "Region.h"

#include "cairo.h"

#ifdef __cplusplus

namespace IBox {

class Canvas {
public:
    Canvas(cairo_t*);
    virtual ~Canvas();

    /** This call saves the current matrix, clip, and drawFilter, and pushes a
        copy onto a private stack. Subsequent calls to translate, scale,
        rotate, skew, concat or clipRect, clipPath, and setDrawFilter all
        operate on this copy.
        When the balancing call to restore() is made, the previous matrix, clip,
        and drawFilter are restored.
        @return The value to pass to restoreToCount() to balance this save()
    */
    virtual int save(/*SaveFlags flags = kMatrixClip_SaveFlag*/);

    /** This call balances a previous call to save(), and is used to remove all
        modifications to the matrix/clip/drawFilter state since the last save
        call.
        It is an error to call restore() more times than save() was called.
    */
    virtual void restore();

    /** Returns the number of matrix/clip states on the SkCanvas' private stack.
        This will equal # save() calls - # restore() calls.
    */
    int getSaveCount() const;

    /** Efficient way to pop any calls to save() that happened after the save
        count reached saveCount. It is an error for saveCount to be less than
        getSaveCount()
        @param saveCount    The number of save() levels to restore from
    */
    void restoreToCount(int saveCount);

    /** Preconcat the current matrix with the specified translation
        @param dx   The distance to translate in X
        @param dy   The distance to translate in Y
        returns true if the operation succeeded (e.g. did not overflow)
    */
    virtual bool translate(int dx, int dy);

    /** Modify the current clip with the specified rectangle.
        @param rect The rect to intersect with the current clip
        @param op The region op to apply to the current clip
        @return true if the canvas' clip is non-empty
    */
    virtual bool clipRect(const Rect& rect,
                          Region::Op op = Region::kIntersect_Op);

    /** Modify the current clip with the specified region. Note that unlike
        clipRect() and clipPath() which transform their arguments by the current
        matrix, clipRegion() assumes its argument is already in device
        coordinates, and so no transformation is performed.
        @param deviceRgn    The region to apply to the current clip
        @param op The region op to apply to the current clip
        @return true if the canvas' new clip is non-empty
    */
    virtual bool clipRegion(const Region& deviceRgn,
                            Region::Op op = Region::kIntersect_Op);

    /** Return true if the specified rectangle, after being transformed by the
        current matrix, would lie completely outside of the current clip. Call
        this to check if an area you intend to draw into is clipped out (and
        therefore you can skip making the draw calls).
        @param rect the rect to compare with the current clip
        @param et  specifies how to treat the edges (see EdgeType)
        @return true if the rect (transformed by the canvas' matrix) does not
                     intersect with the canvas' clip
    */
    bool quickReject(const Rect& rect/*, EdgeType et*/) const;

    cairo_t* mCairo;

private:
    int mSaveCount;
};

/** Stack helper class to automatically call restoreToCount() on the canvas
    when this object goes out of scope. Use this to guarantee that the canvas
    is restored to a known state.
*/
class AutoCanvasRestore : Noncopyable {
public:
    AutoCanvasRestore(Canvas* canvas, bool doSave) : fCanvas(canvas) {
        //SkASSERT(canvas);
        fSaveCount = canvas->getSaveCount();
        if (doSave) {
            canvas->save();
        }
    }
    ~AutoCanvasRestore() {
        fCanvas->restoreToCount(fSaveCount);
    }

private:
    Canvas* fCanvas;
    int     fSaveCount;
};

} // namespace IBox

#endif // __cplusplus

#endif // _Canvas_H_
