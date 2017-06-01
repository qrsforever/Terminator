#ifndef _Rect_H_
#define _Rect_H_

#include "Point.h"

#include <stdint.h>
#include <memory.h>

#ifdef __cplusplus

#include <algorithm>

namespace IBox {

struct Rect {
    int32_t fLeft, fTop, fRight, fBottom;

    /** Return true if the rectangle's width or height are <= 0
    */
    bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }

    /** Returns the rectangle's width. This does not check for a valid rectangle (i.e. left <= right)
        so the result may be negative.
    */
    int width() const { return fRight - fLeft; }

    /** Returns the rectangle's height. This does not check for a valid rectangle (i.e. top <= bottom)
        so the result may be negative.
    */
    int height() const { return fBottom - fTop; }

    friend int operator==(const Rect& a, const Rect& b) {
        return !memcmp(&a, &b, sizeof(a));
    }

    friend int operator!=(const Rect& a, const Rect& b) {
        return memcmp(&a, &b, sizeof(a));
    }

    //bool is16Bit() const {
    //    return  SkIsS16(fLeft) && SkIsS16(fTop) &&
    //            SkIsS16(fRight) && SkIsS16(fBottom);
    //}

    /** Set the rectangle to (0,0,0,0)
    */
    void setEmpty() { memset(this, 0, sizeof(*this)); }

    void set(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        fLeft   = left;
        fTop    = top;
        fRight  = right;
        fBottom = bottom;
    }

    /** Offset set the rectangle by adding dx to its left and right,
        and adding dy to its top and bottom.
    */
    void offset(int32_t dx, int32_t dy) {
        fLeft   += dx;
        fTop    += dy;
        fRight  += dx;
        fBottom += dy;
    }

    void offset(const Point& delta) {
        this->offset(delta.fX, delta.fY);
    }

    /** Inset the rectangle by (dx,dy). If dx is positive, then the sides are moved inwards,
        making the rectangle narrower. If dx is negative, then the sides are moved outwards,
        making the rectangle wider. The same hods true for dy and the top and bottom.
    */
    void inset(int32_t dx, int32_t dy) {
        fLeft   += dx;
        fTop    += dy;
        fRight  -= dx;
        fBottom -= dy;
    }

    /** Returns true if (x,y) is inside the rectangle and the rectangle is not
        empty. The left and top are considered to be inside, while the right
        and bottom are not. Thus for the rectangle (0, 0, 5, 10), the
        points (0,0) and (0,9) are inside, while (-1,0) and (5,9) are not.
    */
    bool contains(int32_t x, int32_t y) const {
        return  (unsigned)(x - fLeft) < (unsigned)(fRight - fLeft) &&
                (unsigned)(y - fTop) < (unsigned)(fBottom - fTop);
    }

    /** Returns true if the 4 specified sides of a rectangle are inside or equal to this rectangle.
        If either rectangle is empty, contains() returns false.
    */
    bool contains(int32_t left, int32_t top, int32_t right, int32_t bottom) const {
        return  left < right && top < bottom && !this->isEmpty() && // check for empties
                fLeft <= left && fTop <= top &&
                fRight >= right && fBottom >= bottom;
    }

    /** Returns true if the specified rectangle r is inside or equal to this rectangle.
    */
    bool contains(const Rect& r) const {
        return  !r.isEmpty() && !this->isEmpty() &&     // check for empties
                fLeft <= r.fLeft && fTop <= r.fTop &&
                fRight >= r.fRight && fBottom >= r.fBottom;
    }

    /** Return true if this rectangle contains the specified rectangle.
		For speed, this method does not check if either this or the specified
		rectangles are empty, and if either is, its return value is undefined.
		In the debugging build however, we assert that both this and the
		specified rectangles are non-empty.
    */
    bool containsNoEmptyCheck(int32_t left, int32_t top,
							  int32_t right, int32_t bottom) const {
        //SkASSERT(fLeft < fRight && fTop < fBottom);
        //SkASSERT(left < right && top < bottom);

        return fLeft <= left && fTop <= top &&
			   fRight >= right && fBottom >= bottom;
    }
    
    /** If r intersects this rectangle, return true and set this rectangle to that
        intersection, otherwise return false and do not change this rectangle.
        If either rectangle is empty, do nothing and return false.
    */
    bool intersect(const Rect& r) {
        //SkASSERT(&r);
        return this->intersect(r.fLeft, r.fTop, r.fRight, r.fBottom);
    }

    /** If rectangles a and b intersect, return true and set this rectangle to
        that intersection, otherwise return false and do not change this
        rectangle. If either rectangle is empty, do nothing and return false.
    */
    bool intersect(const Rect& a, const Rect& b) {
        //SkASSERT(&a && &b);
        
        if (!a.isEmpty() && !b.isEmpty() &&
                a.fLeft < b.fRight && b.fLeft < a.fRight &&
                a.fTop < b.fBottom && b.fTop < a.fBottom) {
            fLeft   = std::max(a.fLeft,   b.fLeft);
            fTop    = std::max(a.fTop,    b.fTop);
            fRight  = std::min(a.fRight,  b.fRight);
            fBottom = std::min(a.fBottom, b.fBottom);
            return true;
        }
        return false;
    }
    
    /** If rectangles a and b intersect, return true and set this rectangle to
        that intersection, otherwise return false and do not change this
        rectangle. For speed, no check to see if a or b are empty is performed.
        If either is, then the return result is undefined. In the debug build,
        we assert that both rectangles are non-empty.
    */
    bool intersectNoEmptyCheck(const Rect& a, const Rect& b) {
        //SkASSERT(&a && &b);
        //SkASSERT(!a.isEmpty() && !b.isEmpty());
        
        if (a.fLeft < b.fRight && b.fLeft < a.fRight &&
                a.fTop < b.fBottom && b.fTop < a.fBottom) {
            fLeft   = std::max(a.fLeft,   b.fLeft);
            fTop    = std::max(a.fTop,    b.fTop);
            fRight  = std::min(a.fRight,  b.fRight);
            fBottom = std::min(a.fBottom, b.fBottom);
            return true;
        }
        return false;
    }

    /** If the rectangle specified by left,top,right,bottom intersects this rectangle,
        return true and set this rectangle to that intersection,
        otherwise return false and do not change this rectangle.
        If either rectangle is empty, do nothing and return false.
    */
    bool intersect(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        if (left < right && top < bottom && !this->isEmpty() &&
                fLeft < right && left < fRight && fTop < bottom && top < fBottom) {
            if (fLeft < left) fLeft = left;
            if (fTop < top) fTop = top;
            if (fRight > right) fRight = right;
            if (fBottom > bottom) fBottom = bottom;
            return true;
        }
        return false;
    }
    
    /** Returns true if a and b are not empty, and they intersect
    */
    static bool Intersects(const Rect& a, const Rect& b) {
        return  !a.isEmpty() && !b.isEmpty() &&              // check for empties
                a.fLeft < b.fRight && b.fLeft < a.fRight &&
                a.fTop < b.fBottom && b.fTop < a.fBottom;
    }
    
    /** Update this rectangle to enclose itself and the specified rectangle.
        If this rectangle is empty, just set it to the specified rectangle. If the specified
        rectangle is empty, do nothing.
    */
    void join(int32_t left, int32_t top, int32_t right, int32_t bottom);

    /** Update this rectangle to enclose itself and the specified rectangle.
        If this rectangle is empty, just set it to the specified rectangle. If the specified
        rectangle is empty, do nothing.
    */
    void join(const Rect& r) {
        this->join(r.fLeft, r.fTop, r.fRight, r.fBottom);
    }

    /** Swap top/bottom or left/right if there are flipped.
        This can be called if the edges are computed separately,
        and may have crossed over each other.
        When this returns, left <= right && top <= bottom
    */
    void sort();
};

} // namespace IBox

#endif // __cplusplus

#endif // _Rect_H_
