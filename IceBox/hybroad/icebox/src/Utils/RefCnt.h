#ifndef _RefCnt_H_
#define _RefCnt_H_

#include "Types.h"

#ifdef __cplusplus

namespace IBox {

/*
RefCnt is the base class for objects that may be shared by multiple
objects. When a new owner wants a reference, it calls ref(). When an owner
wants to release its reference, it calls unref(). When the shared object's
reference count goes to zero as the result of an unref() call, its (virtual)
destructor is called. It is an error for the destructor to be called
explicitly (or via the object going out of scope on the stack or calling
delete) if getRefCnt() > 1.
*/
class RefCnt : Noncopyable {
public:
    /** Default construct, initializing the reference count to 1.
    */
    RefCnt() : mRefCnt(1) {}

    /**  Destruct, asserting that the reference count is 1.
    */
    virtual ~RefCnt() { /*SkASSERT(mRefCnt == 1);*/ }

    /** Return the reference count.
    */
    int getRefCnt() const { return mRefCnt; }

    /** Increment the reference count. Must be balanced by a call to unref().
    */
    void ref() const {
        /*SkASSERT(mRefCnt > 0);*/
        mRefCnt++;/*sk_atomic_inc(&mRefCnt);*/
    }

    /** Decrement the reference count. If the reference count is 1 before the
        decrement, then call delete on the object. Note that if this is the
        case, then the object needs to have been allocated via new, and not on
        the stack.
    */
    void unref() const {
        /*SkASSERT(mRefCnt > 0);*/
        if (mRefCnt--/*sk_atomic_dec(&mRefCnt)*/ == 1) {
            mRefCnt = 1;    // so our destructor won't complain
            delete this;
        }
    }
    
    /** Helper version of ref(), that first checks to see if this is not null.
        If this is null, then do nothing.
    */
    void safeRef() const {
        if (this) {
            this->ref();
        }
    }

    /** Helper version of unref(), that first checks to see if this is not null.
        If this is null, then do nothing.
    */
    void safeUnref() const {
        if (this) {
            this->unref();
        }
    }

private:
    mutable int mRefCnt;
};

/*
AutoUnref is a stack-helper class that will automatically call unref() on
the object it points to when the AutoUnref object goes out of scope.
If obj is null, do nothing.
*/
class AutoUnref : Noncopyable {
public:
    AutoUnref(RefCnt* obj) : mObj(obj) {}
    ~AutoUnref();

    RefCnt*   get() const { return mObj; }

    /** If the hosted object is null, do nothing and return false, else call
        ref() on it and return true
    */
    bool      ref();

    /** If the hosted object is null, do nothing and return false, else call
        unref() on it, set its reference to null, and return true
    */
    bool      unref();

    /** If the hosted object is null, do nothing and return NULL, else call
        unref() on it, set its reference to null, and return the object
    */
    RefCnt*   detach();

private:
    RefCnt*   mObj;
};

} // namespace IBox

#endif // __cplusplus

#endif // _RefCnt_H_
