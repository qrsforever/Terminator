#include "Region.h"

namespace IBox {

Region::Region()
{
    pixman_region32_init(&mImplement);
}

Region::~Region()
{
    pixman_region32_fini(&mImplement);
}

const Rect
Region::getBounds()
{
    pixman_box32_t* extents = pixman_region32_extents(&mImplement);
    Rect r;
    if (extents)
        r.set(extents->x1, extents->y1, extents->x2, extents->y2);
    else
        r.set(0, 0, 0, 0);
    return r;
}

bool 
Region::isEmpty()
{
    pixman_box32_t* extents = pixman_region32_extents(&mImplement);
    return (extents->x1 == 0 && extents->y1 == 0 &&  extents->x2 == 0 && extents->y2 == 0);
}

bool 
Region::setEmpty()
{
    pixman_box32_t box = {0, 0, 0, 0};
    pixman_region32_reset(&mImplement, &box);
    return true;
}

bool 
Region::op(/*const*/ Region& rgn, const Rect& rect, Op ParamOp)
{
    if (ParamOp == kUnion_Op)
        return pixman_region32_union_rect(&rgn.mImplement, &rgn.mImplement, rect.fLeft, rect.fTop, rect.fRight - rect.fLeft, rect.fBottom - rect.fTop);
    else
        return false;
}

} // namespace IBox
