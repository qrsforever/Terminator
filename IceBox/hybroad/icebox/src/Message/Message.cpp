
#include "Message.h"
#include "MessageHandler.h"
#include "Runnable.h"

#include <pthread.h>


namespace IBox {

#ifndef MESSAGE_MAX_POOL_SIZE
#define MESSAGE_MAX_POOL_SIZE	100
#endif

static Message gMessages[MESSAGE_MAX_POOL_SIZE];

static inline int inHeap(Message *msg)
{
    if ((msg >= &gMessages[0]) && (msg < &gMessages[sizeof(gMessages)/sizeof(gMessages[0])]))
        return 0;
    else
        return 1;
}

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

Message *Message::mPool = NULL;
int Message::mPoolSize = 0;

/**
 * Return a new Message instance from the global pool. Allows us to
 * avoid allocating new objects in many cases.
 */
Message *
Message::obtain()
{
    Message *m;

    pthread_mutex_lock(&gMutex);

    if (mPoolSize < MESSAGE_MAX_POOL_SIZE) {
        m = &gMessages[mPoolSize++];
    }
    else if (mPool != NULL) {
        m = mPool;
        mPool = m->next;
        m->next = NULL;
    }
    else
        m = new Message();

    pthread_mutex_unlock(&gMutex);

    return m;
}

/**
 * Same as {@link #obtain()}, but sets the value for the <em>target</em> member on the Message returned.
 * @param h  Handler to assign to the returned Message object's <em>target</em> member.
 * @return A Message object from the global pool.
 */
Message *
Message::obtain(MessageHandler *h)
{
    Message *m = obtain();
    m->target = h;
    return m;
}

/**
 * Same as {@link #obtain()}, but sets the values for both <em>target</em> and
 * <em>what</em> members on the Message.
 * @param h  Value to assign to the <em>target</em> member.
 * @param pWhat  Value to assign to the <em>what</em> member.
 * @return A Message object from the global pool.
 */
Message *
Message::obtain(MessageHandler *h, int pWhat)
{
    Message *m = obtain();
    m->target = h;
    m->what = pWhat;
    return m;
}

/**
 * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, and <em>obj</em>
 * members.
 * @param h  The <em>target</em> value to set.
 * @param pWhat  The <em>what</em> value to set.
 * @param pObj  The <em>object</em> method to set.
 * @return  A Message object from the global pool.
 */
Message *
Message::obtain(MessageHandler *h, int pWhat, Object *pObj)
{
    Message *m = obtain();
    m->target = h;
    m->what = pWhat;
    m->obj = pObj;
    m->obj->safeRef();
    return m;
}

/**
 * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, 
 * <em>arg1</em>, and <em>arg2</em> members.
 * 
 * @param h  The <em>target</em> value to set.
 * @param pWhat  The <em>pWhat</em> value to set.
 * @param pArg1  The <em>pArg1</em> value to set.
 * @param pArg2  The <em>pArg2</em> value to set.
 * @return  A Message object from the global pool.
 */
Message *
Message::obtain(MessageHandler *h, int pWhat, int pArg1, int pArg2)
{
    Message *m = obtain();
    m->target = h;
    m->what = pWhat;
    m->arg1 = pArg1;
    m->arg2 = pArg2;
    return m;
}

/**
 * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, 
 * <em>arg1</em>, <em>arg2</em>, and <em>obj</em> members.
 * 
 * @param h  The <em>target</em> value to set.
 * @param pWhat  The <em>pWhat</em> value to set.
 * @param pArg1  The <em>pArg1</em> value to set.
 * @param pArg2  The <em>pArg2</em> value to set.
 * @param pObj  The <em>pObj</em> value to set.
 * @return  A Message object from the global pool.
 */
Message *
Message::obtain(MessageHandler *h, int pWhat, int pArg1, int pArg2, Object *pObj)
{
    Message *m = obtain();
    m->target = h;
    m->what = pWhat;
    m->arg1 = pArg1;
    m->arg2 = pArg2;
    m->obj = pObj;
    m->obj->safeRef();
    return m;
}

/**
 * Return a Message instance to the global pool.  You MUST NOT touch
 * the Message after calling this function -- it has effectively been
 * freed.
 */
void 
Message::recycle()
{
    clearForRecycle();

    if (!inHeap(this)) {
        pthread_mutex_lock(&gMutex);
        next = mPool;
        mPool = this;
        pthread_mutex_unlock(&gMutex);
    }
    else
        delete this;
}

/**
 * Sends this Message to the Handler specified by {@link #getTarget}.
 * Throws a null pointer exception if this field has not been set.
 */
void 
Message::sendToTarget()
{
    target->sendMessage(this);
}

void 
Message::clearForRecycle()
{
    //what = 0;
    //arg1 = 0;
    //arg2 = 0;
    obj->safeUnref();
    obj = NULL;
    //replyTo = NULL;
    when = 0;
    target = NULL;
    callback->safeUnref();
    callback = NULL;
    //data = NULL;
}

/** Constructor (but the preferred way to get a Message is to call {@link #obtain() Message.obtain()}).
*/
Message::Message()
{
    what = 0;
    arg1 = 0;
    arg2 = 0;
    obj = NULL;
    //replyTo = NULL;
    when = 0;
    target = NULL;
    callback = NULL;
    //data = NULL;
    next = NULL;
}

Message::~Message()
{
}

} // namespace IBox
