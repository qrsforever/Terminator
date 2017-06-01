#include "Canvas.h"

namespace IBox {

Canvas::Canvas(cairo_t* cr)
	: mCairo(cr)
	, mSaveCount(0)
{
}

Canvas::~Canvas()
{
}

int 
Canvas::save()
{
    cairo_save(mCairo);
    return ++mSaveCount;
}

void 
Canvas::restore()
{
    if (mSaveCount > 0) {
        --mSaveCount;
        cairo_restore(mCairo);
    }
}

int 
Canvas::getSaveCount() const
{
    return mSaveCount;
}

void 
Canvas::restoreToCount(int saveCount)
{
    for (; mSaveCount > saveCount; mSaveCount--)
        cairo_restore(mCairo);
}

bool 
Canvas::translate(int dx, int dy)
{
    cairo_translate(mCairo, dx, dy);
    return true;
}

bool 
Canvas::clipRect(const Rect& rect, Region::Op op)
{
    cairo_rectangle(mCairo, rect.fLeft, rect.fTop, rect.width(), rect.height());

    cairo_fill_rule_t savedFillRule = cairo_get_fill_rule(mCairo);
    cairo_set_fill_rule(mCairo, CAIRO_FILL_RULE_WINDING);
    cairo_clip(mCairo);
    cairo_set_fill_rule(mCairo, savedFillRule);

    return true;
}

bool 
Canvas::clipRegion(const Region& rgn, Region::Op op)
{
    int n;
    pixman_box32_t* boxes = pixman_region32_rectangles((pixman_region32_t *)&rgn.mImplement, &n);
    for (int i = 0; i < n; i++) {
        cairo_rectangle(mCairo, boxes[i].x1, boxes[i].y1, boxes[i].x2 - boxes[i].x1, boxes[i].y2 - boxes[i].y1);
    }

    cairo_fill_rule_t savedFillRule = cairo_get_fill_rule(mCairo);
    cairo_set_fill_rule(mCairo, CAIRO_FILL_RULE_WINDING);
    cairo_clip(mCairo);
    cairo_set_fill_rule(mCairo, savedFillRule);

    return true;
}

bool 
Canvas::quickReject(const Rect& rect) const
{
    double x1, x2, y1, y2;
    cairo_clip_extents(mCairo, &x1, &y1, &x2, &y2);

    Rect extents;
    extents.set((int)x1, (int)y1, (int)x2, (int)y2);
    return !extents.intersect(rect);
}

} // namespace IBox
