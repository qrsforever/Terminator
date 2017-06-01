#include "Rect.h"

namespace IBox {

void Rect::join(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    // do nothing if the params are empty
    if (left >= right || top >= bottom)
        return;

    // if we are empty, just assign
    if (fLeft >= fRight || fTop >= fBottom)
        this->set(left, top, right, bottom);
    else
    {
        if (left < fLeft) fLeft = left;
        if (top < fTop) fTop = top;
        if (right > fRight) fRight = right;
        if (bottom > fBottom) fBottom = bottom;
    }
}

void Rect::sort()
{
    if (fLeft > fRight) {
        //SkTSwap<int32_t>(fLeft, fRight);
        int32_t tmp = fLeft;
        fLeft = fRight;
        fRight = tmp;
    }
    if (fTop > fBottom) {
        //SkTSwap<int32_t>(fTop, fBottom);
        int32_t tmp = fTop;
        fTop = fBottom;
        fBottom = tmp;
    }
}

} // namespace IBox
