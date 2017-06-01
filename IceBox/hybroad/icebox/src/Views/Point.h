#ifndef _Point_H_
#define _Point_H_

#include <stdint.h>

#ifdef __cplusplus

namespace IBox {

struct Point {
    int32_t fX, fY;
    
    static Point Make(int32_t x, int32_t y) {
        Point pt;
        pt.set(x, y);
        return pt;
    }

    /** Set the x and y values of the point. */
    void set(int32_t x, int32_t y) { fX = x; fY = y; }

    /** Rotate the point clockwise, writing the new point into dst
        It is legal for dst == this
    */
    void rotateCW(Point* dst) const;

    /** Rotate the point clockwise, writing the new point back into the point
    */

    void rotateCW() { this->rotateCW(this); }

    /** Rotate the point counter-clockwise, writing the new point into dst.
        It is legal for dst == this
    */
    void rotateCCW(Point* dst) const;

    /** Rotate the point counter-clockwise, writing the new point back into
        the point
    */
    void rotateCCW() { this->rotateCCW(this); }
    
    /** Negate the X and Y coordinates of the point.
    */
    void negate() { fX = -fX; fY = -fY; }
    
    /** Return a new point whose X and Y coordinates are the negative of the
        original point's
    */
    Point operator-() const {
        Point neg;
        neg.fX = -fX;
        neg.fY = -fY;
        return neg;
    }

    /** Add v's coordinates to this point's */
    void operator+=(const Point& v) {
        fX += v.fX;
        fY += v.fY;
    }
    
    /** Subtract v's coordinates from this point's */
    void operator-=(const Point& v) {
        fX -= v.fX;
        fY -= v.fY;
    }

    /** Returns true if the point's coordinates equal (x,y) */
    bool equals(int32_t x, int32_t y) const {
        return fX == x && fY == y;
    }

    friend bool operator==(const Point& a, const Point& b) {
        return a.fX == b.fX && a.fY == b.fY;
    }
    
    friend bool operator!=(const Point& a, const Point& b) {
        return a.fX != b.fX || a.fY != b.fY;
    }

    /** Returns a new point whose coordinates are the difference between
        a and b (i.e. a - b)
    */
    friend Point operator-(const Point& a, const Point& b) {
        Point v;
        v.set(a.fX - b.fX, a.fY - b.fY);
        return v;
    }

    /** Returns a new point whose coordinates are the sum of a and b (a + b)
    */
    friend Point operator+(const Point& a, const Point& b) {
        Point v;
        v.set(a.fX + b.fX, a.fY + b.fY);
        return v;
    }
    
    /** Returns the dot product of a and b, treating them as 2D vectors
    */
    static int32_t DotProduct(const Point& a, const Point& b) {
        return a.fX * b.fX + a.fY * b.fY;
    }

    /** Returns the cross product of a and b, treating them as 2D vectors
    */
    static int32_t CrossProduct(const Point& a, const Point& b) {
        return a.fX * b.fY - a.fY * b.fX;
    }
};

} // namespace IBox

#endif // __cplusplus

#endif // _Point_H_
