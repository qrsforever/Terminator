#include "View.h"
#include "Canvas.h"

namespace IBox {

////////////////////////////////////////////////////////////////////////

View::View(uint32_t flags) : fFlags((uint8_t)flags)
{
	fWidth = fHeight = 0;
	fLoc.set(0, 0);
	fParent = fFirstChild = fNextSibling = fPrevSibling = NULL;

	fContainsFocus = 0;
}

View::~View()
{
	this->detachAllChildren();
}

void View::setFlags(uint32_t flags)
{
	//SkASSERT((flags & ~kAllFlagMasks) == 0);

	uint32_t diff = fFlags ^ flags;

	if (diff & kVisible_Mask)
		this->inval(NULL);

	fFlags = (uint8_t)flags;

	if (diff & kVisible_Mask) {
		this->inval(NULL);
	}
}

static inline uint32_t SetClearShift(uint32_t bits, bool cond,
                                       unsigned shift) {
    //SkASSERT((int)cond == 0 || (int)cond == 1);
    return (bits & ~(1 << shift)) | ((int)cond << shift);
}

void View::setVisibleP(bool pred)
{
	this->setFlags(SetClearShift(fFlags, pred, kVisible_Shift));
}

void View::setEnabledP(bool pred)
{
	this->setFlags(SetClearShift(fFlags, pred, kEnabled_Shift));
}

void View::setFocusableP(bool pred)
{
	this->setFlags(SetClearShift(fFlags, pred, kFocusable_Shift));
}

void View::setSize(int32_t pWidth, int32_t pHeight)
{
	pWidth = std::max(0, pWidth);
	pHeight = std::max(0, pHeight);

	if (fWidth != pWidth || fHeight != pHeight) {
		this->inval(NULL);
		fWidth = pWidth;
		fHeight = pHeight;
		this->inval(NULL);
		this->onSizeChange();
		this->invokeLayout();
	}
}

void View::setLoc(int32_t x, int32_t y)
{
	if (fLoc.fX != x || fLoc.fY != y) {
		this->inval(NULL);
		fLoc.set(x, y);
		this->inval(NULL);
	}
}

void View::offset(int32_t dx, int32_t dy)
{
	if (dx || dy)
		this->setLoc(fLoc.fX + dx, fLoc.fY + dy);
}

void View::draw(Canvas* canvas)
{
	if (fWidth && fHeight && this->isVisible()) {
        Rect r;

        r.set(fLoc.fX, fLoc.fY, fLoc.fX + fWidth, fLoc.fY + fHeight);
        if (canvas->quickReject(r/*, SkCanvas::kBW_EdgeType*/)) 
            return;

		AutoCanvasRestore as(canvas, true);

		canvas->clipRect(r);
		canvas->translate(fLoc.fX, fLoc.fY);

        //if (fParent) {
        //    fParent->beforeChild(this, canvas);
        //}

        int sc = canvas->save();
		this->onDraw(canvas);
        canvas->restoreToCount(sc);

        //if (fParent) {
        //    fParent->afterChild(this, canvas);
        //}

		B2FIter	iter(this);
		View*	child;

        Canvas* childCanvas = canvas; //this->beforeChildren(canvas);

		while ((child = iter.next()) != NULL) {
			child->draw(childCanvas);
        }

        //this->afterChildren(canvas);
	}
}

void View::inval(Rect* rect)
{
	if (!this->isVisible())
		return;

	Rect bounds;

	this->getLocalBounds(&bounds);
	if (rect && !bounds.intersect(*rect))
		return;

	rect = &bounds;
	View* view = this;

	for (;;) {
		if (view->handleInval(bounds))
			break;

		Rect parentR;
		View* parent = view->fParent;

		if (parent == NULL || !parent->isVisible())
			break;

		bounds.offset(view->fLoc.fX, view->fLoc.fY);
		parent->getLocalBounds(&parentR);
		if (!bounds.intersect(parentR))
			return;

		view = parent;
	}
}

//////////////////////////////////////////////////////////////////////

void View::invokeLayout() {
	View::Layout* layout = this->getLayout();

	if (layout) {
		layout->layoutChildren(this);
    }
}

void View::onDraw(Canvas* canvas) {
	//Artist* artist = this->getArtist();

	//if (artist) {
	//	artist->draw(this, canvas);
    //}
}

void View::onSizeChange() {}

bool View::handleInval(const Rect& r) {
	return false;
}

//////////////////////////////////////////////////////////////////////

void View::getLocalBounds(Rect* bounds) const
{
	if (bounds)
		bounds->set(0, 0, fWidth, fHeight);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void View::detachFromParent_NoLayout()
{
	if (fParent == NULL)
		return;

	//if (fContainsFocus)
	//	(void)this->setFocusView(NULL);

	this->inval(NULL);

	View*	next = NULL;

	if (fNextSibling != this) {	// do we have any siblings
		fNextSibling->fPrevSibling = fPrevSibling;
		fPrevSibling->fNextSibling = fNextSibling;
		next = fNextSibling;
	}

	if (fParent->fFirstChild == this)
		fParent->fFirstChild = next;

	fParent = fNextSibling = fPrevSibling = NULL;

	this->unref();
}

void View::detachFromParent()
{
	View* parent = fParent;

	if (parent) {
		this->detachFromParent_NoLayout();
		parent->invokeLayout();
	}
}

View* View::attachChildToBack(View* child)
{
	//SkASSERT(child != this);

	if (child == NULL || fFirstChild == child)
		goto DONE;

	child->ref();
	child->detachFromParent_NoLayout();

	if (fFirstChild == NULL) {
		child->fNextSibling = child;
		child->fPrevSibling = child;
	} else {
		child->fNextSibling = fFirstChild;
		child->fPrevSibling = fFirstChild->fPrevSibling;
		fFirstChild->fPrevSibling->fNextSibling = child;
		fFirstChild->fPrevSibling = child;
	}

	fFirstChild = child;
	child->fParent = this;
	child->inval(NULL);

	this->invokeLayout();
DONE:
	return child;
}

View* View::attachChildToFront(View* child)
{
	//SkASSERT(child != this);

	if (child == NULL || (fFirstChild && fFirstChild->fPrevSibling == child))
		goto DONE;

	child->ref();
	child->detachFromParent_NoLayout();

	if (fFirstChild == NULL) {
		fFirstChild = child;
		child->fNextSibling = child;
		child->fPrevSibling = child;
	} else {
		child->fNextSibling = fFirstChild;
		child->fPrevSibling = fFirstChild->fPrevSibling;
		fFirstChild->fPrevSibling->fNextSibling = child;
		fFirstChild->fPrevSibling = child;
	}

	child->fParent = this;
	child->inval(NULL);

	this->invokeLayout();
DONE:
	return child;
}

void View::detachAllChildren()
{
	while (fFirstChild)
		fFirstChild->detachFromParent_NoLayout();
}

void View::globalToLocal(int32_t x, int32_t y, Point* local) const
{
	//SkASSERT(this);
	if (local) {
		const View* view = this;
		while (view) {
			x -= view->fLoc.fX;
			y -= view->fLoc.fY;
			view = view->fParent;
		}
		local->set(x, y);
	}
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

View::F2BIter::F2BIter(const View* parent)
{
	fFirstChild = parent ? parent->fFirstChild : NULL;
	fChild = fFirstChild ? fFirstChild->fPrevSibling : NULL;
}

View* View::F2BIter::next()
{
	View* curr = fChild;

	if (fChild) {
		if (fChild == fFirstChild)
			fChild = NULL;
		else
			fChild = fChild->fPrevSibling;
	}
	return curr;
}

View::B2FIter::B2FIter(const View* parent)
{
	fFirstChild = parent ? parent->fFirstChild : NULL;
	fChild = fFirstChild;
}

View* View::B2FIter::next()
{
	View* curr = fChild;

	if (fChild) {
		View *tNext = fChild->fNextSibling;

		if (tNext == fFirstChild)
			tNext = NULL;
		fChild = tNext;
	}
	return curr;
}

} // namespace IBox

