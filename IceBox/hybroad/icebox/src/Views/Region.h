#ifndef _Region_H_
#define _Region_H_

#include "Rect.h"

extern "C" {
#include "pixman.h"
}

#ifdef __cplusplus

namespace IBox {

class Region {
public:
    Region();
    ~Region();

    /** Return the bounds of this region. If the region is empty, returns an
        empty rectangle.
    */
    const Rect getBounds(); /*const { return fBounds; }*/

    /** Return true if this region is empty */
    bool isEmpty();

    /** Set the region to be empty, and return false, since the resulting
        region is empty
    */
    bool setEmpty();

    /** The logical operations that can be performed when combining two regions.
    */
    enum Op {
        kDifference_Op, //!< subtract the op region from the first region
        kIntersect_Op,  //!< intersect the two regions
        kUnion_Op,      //!< union (inclusive-or) the two regions
        kXOR_Op,        //!< exclusive-or the two regions
        /** subtract the first region from the op region */
        kReverseDifference_Op,
        kReplace_Op     //!< replace the dst region with the op region
    };

    /** Set this region to the result of applying the Op to this region and the
        specified rectangle: this = (this op rect).
        Return true if the resulting region is non-empty.
        */
    bool op(const Rect& rect, Op pOp) { return this->op(*this, rect, pOp); }

    /** Set this region to the result of applying the Op to this region and the
        specified rectangle: this = (this op rect).
        Return true if the resulting region is non-empty.
    */
    bool op(int left, int top, int right, int bottom, Op pOp) {
        Rect rect;
        rect.set(left, top, right, bottom);
        return this->op(*this, rect, pOp);
    }

    /** Set this region to the result of applying the Op to the specified
        region and rectangle: this = (rgn op rect).
        Return true if the resulting region is non-empty.
    */
    bool op(/*const*/ Region& rgn, const Rect& rect, Op);

//private:
    pixman_region32_t mImplement;
};

} // namespace IBox

#endif // __cplusplus

#endif // _Region_H_
