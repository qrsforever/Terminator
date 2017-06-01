#ifndef _View_H_
#define _View_H_

#include "Object.h"
#include "Point.h"
#include "Rect.h"

#include <stdint.h>

#include "ViewAssertions.h" //TODO for debug

#ifdef __cplusplus

namespace IBox {

class Canvas;

class View : public Object {
public:
    enum Flag_Shift {
        kVisible_Shift,
        kEnabled_Shift,
        kFocusable_Shift,
        kFlexH_Shift,
        kFlexV_Shift,

        kFlagShiftCount
    };
    enum Flag_Mask {
        kVisible_Mask   = 1 << kVisible_Shift,      //!< set if the view is visible
        kEnabled_Mask   = 1 << kEnabled_Shift,      //!< set if the view is enabled
        kFocusable_Mask = 1 << kFocusable_Shift,    //!< set if the view can receive focus
        kFlexH_Mask     = 1 << kFlexH_Shift,        //!< set if the view's width is stretchable
        kFlexV_Mask     = 1 << kFlexV_Shift,        //!< set if the view's height is stretchable

        kAllFlagMasks   = (uint32_t)(0 - 1) >> (32 - kFlagShiftCount)
    };

                View(uint32_t flags = 0);
    virtual     ~View();
    virtual     int typeID() { return 0; } //FOR DEBUG lidong add.

    /** Return the flags associated with the view
    */
    uint32_t    getFlags() const { return fFlags; }
    /** Set the flags associated with the view
    */
    void        setFlags(uint32_t flags);

    /** Helper that returns non-zero if the kVisible_Mask bit is set in the view's flags
    */
    int         isVisible() const { return fFlags & kVisible_Mask; }
    int         isEnabled() const { return fFlags & kEnabled_Mask; }
    int         isFocusable() const { return fFlags & kFocusable_Mask; }
    /** Helper to set/clear the view's kVisible_Mask flag */
    void        setVisibleP(bool);
    void        setEnabledP(bool);
    void        setFocusableP(bool);

    /** Return the view's width */
    int32_t     width() const { return fWidth; }
    /** Return the view's height */
    int32_t     height() const { return fHeight; }
    /** Set the view's width and height. These must both be >= 0. This does not affect the view's loc */
    void        setSize(int32_t pWidth, int32_t pHeight);
    void        setSize(const Point& size) { this->setSize(size.fX, size.fY); }
    void        setWidth(int32_t pWidth) { this->setSize(pWidth, fHeight); }
    void        setHeight(int32_t pHeight) { this->setSize(fWidth, pHeight); }
    /** Return a rectangle set to [0, 0, width, height] */
    void        getLocalBounds(Rect* bounds) const;

    /** Return the view's left edge */
    int32_t     locX() const { return fLoc.fX; }
    /** Return the view's top edge */
    int32_t     locY() const { return fLoc.fY; }
    /** Set the view's left and top edge. This does not affect the view's size */
    void        setLoc(int32_t x, int32_t y);
    void        setLoc(const Point& loc) { this->setLoc(loc.fX, loc.fY); }
    void        setLocX(int32_t x) { this->setLoc(x, fLoc.fY); }
    void        setLocY(int32_t y) { this->setLoc(fLoc.fX, y); }
    /** Offset (move) the view by the specified dx and dy. This does not affect the view's size */
    void        offset(int32_t dx, int32_t dy);

    /** Call this to have the view draw into the specified canvas. */
    virtual void draw(Canvas* canvas);

    /** Call this to invalidate part of all of a view, requesting that the view's
        draw method be called. The rectangle parameter specifies the part of the view
        that should be redrawn. If it is null, it specifies the entire view bounds.
    */
    void        inval(Rect* rectOrNull);

    //  View hierarchy management

    /** Return the view's parent, or null if it has none. This does not affect the parent's reference count. */
    View*       getParent() const { return fParent; }
    View*       attachChildToFront(View* child);
    /** Attach the child view to this view, and increment the child's reference count. The child view is added
        such that it will be drawn before all other child views.
        The child view parameter is returned.
    */
    View*       attachChildToBack(View* child);
    /** If the view has a parent, detach the view from its parent and decrement the view's reference count.
        If the parent was the only owner of the view, this will cause the view to be deleted.
    */
    void        detachFromParent();
    /** Attach the child view to this view, and increment the child's reference count. The child view is added
        such that it will be drawn after all other child views.
        The child view parameter is returned.
    */
    /** Detach all child views from this view. */
    void        detachAllChildren();

    /** Convert the specified point from global coordinates into view-local coordinates
    */
    void        globalToLocal(Point* pt) const { if (pt) this->globalToLocal(pt->fX, pt->fY, pt); }
    /** Convert the specified x,y from global coordinates into view-local coordinates, returning
        the answer in the local parameter.
    */
    void        globalToLocal(int32_t globalX, int32_t globalY, Point* local) const;

    /** \class F2BIter
    
        Iterator that will return each of this view's children, in
        front-to-back order (the order used for clicking). The first
        call to next() returns the front-most child view. When
        next() returns null, there are no more child views.
    */
    class F2BIter {
    public:
        F2BIter(const View* parent);
        View* next();
    private:
        View* fFirstChild, *fChild;
    };

    /** \class B2FIter
    
        Iterator that will return each of this view's children, in
        back-to-front order (the order they are drawn). The first
        call to next() returns the back-most child view. When
        next() returns null, there are no more child views.
    */
    class B2FIter {
    public:
        B2FIter(const View* parent);
        View* next();
    private:
        View* fFirstChild, *fChild;
    };

    /** \class Layout
    
        Install a subclass of this in a view (calling setLayout()), and then the
        default implementation of that view's onLayoutChildren() will invoke
        this object automatically.
    */
    class Layout : public RefCnt {
    public:
        void layoutChildren(View* parent);
        //void inflate(const SkDOM&, const SkDOM::Node*);
    protected:
        virtual void onLayoutChildren(View* parent) = 0;
        //virtual void onInflate(const SkDOM&, const SkDOM::Node*);
    };

    /** Return the layout attached to this view (or null). The layout's reference
        count is not affected.
    */
    Layout* getLayout() const;
    /** Attach the specified layout (or null) to the view, replacing any existing
        layout. If the new layout is not null, its reference count is incremented.
        The layout parameter is returned.
    */
    Layout* setLayout(Layout*, bool invokeLayoutNow = true);
    /** If a layout is attached to this view, call its layoutChildren() method
    */
    void    invokeLayout();

protected:
    /** Override this to draw inside the view. Be sure to call the inherited version too */
    virtual void    onDraw(Canvas*);
    /** Override this to be notified when the view's size changes. Be sure to call the inherited version too */
    virtual void    onSizeChange();
    /** Override this if you want to handle an inval request from this view or one of its children.
        Tyically this is only overridden by the by the "window". If your subclass does handle the
        request, return true so the request will not continue to propogate to the parent.
    */
    virtual bool    handleInval(const Rect&);
    //! called once before all of the children are drawn (or clipped/translated)
//    virtual SkCanvas* beforeChildren(SkCanvas* c) { return c; }
    //! called once after all of the children are drawn (or clipped/translated)
//    virtual void afterChildren(SkCanvas* orig) {}

    //! called right before this child's onDraw is called
//    virtual void beforeChild(SkView* child, SkCanvas* canvas) {}
    //! called right after this child's onDraw is called
//    virtual void afterChild(SkView* child, SkCanvas* canvas) {}

private:
    int32_t     fWidth, fHeight;
    Point       fLoc;
    View*       fParent;
    View*       fFirstChild;
    View*       fNextSibling;
    View*       fPrevSibling;
    uint8_t     fFlags;
    uint8_t     fContainsFocus;

    friend class B2FIter;
    friend class F2BIter;

    void    detachFromParent_NoLayout();
};

} // namespace IBox

#endif // __cplusplus

#endif // _View_H_
