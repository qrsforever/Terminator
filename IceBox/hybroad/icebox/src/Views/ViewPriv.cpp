#include "View.h"


namespace IBox {

////////////////////////////////////////////////////////////////////////////////

void View::Layout::layoutChildren(View* parent)
{
	//SkASSERT(parent);
	if (parent->width() > 0 && parent->height() > 0)
		this->onLayoutChildren(parent);
}

View::Layout* View::getLayout() const
{
	return NULL;
}

View::Layout* View::setLayout(Layout* obj, bool invokeLayoutNow)
{
	return obj;
}

} // namespace IBox
