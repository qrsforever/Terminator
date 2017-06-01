#ifndef _Message_H_
#define _Message_H_

#include "Object.h"

#ifdef __cplusplus

namespace IBox {

class MessageHandler;
class Runnable;

class Message {
public:
    /**
     * User-defined message code so that the recipient can identify 
     * what this message is about. Each {@link Handler} has its own name-space
     * for message codes, so you do not need to worry about yours conflicting
     * with other handlers.
     */
    int what;

    // Use these fields instead of using the class's Bundle if you can. 
    /** arg1 and arg2 are lower-cost alternatives to using {@link #setData(Bundle) setData()}
    if you only need to store a few integer values. */
    int arg1;

    /** arg1 and arg2 are lower-cost alternatives to using {@link #setData(Bundle) setData()}
    if you only need to store a few integer values.*/
    int arg2;

    /** An arbitrary object to send to the recipient.  This must be null when
     * sending messages across processes. */
    Object *obj;

    /** Optional Messenger where replies to this message can be sent.
     */
    //public: Messenger replyTo;
    
    unsigned long when;
    
    //Bundle *data;
    
    MessageHandler *target;  
    
    Runnable *callback;
    
    // sometimes we store linked lists of these things
    Message *next;

    /**
     * Return a new Message instance from the global pool. Allows us to
     * avoid allocating new objects in many cases.
     */
    static Message *obtain();

    /**
     * Same as {@link #obtain()}, but copies the values of an existing
     * message (including its target) into the new one.
     * @param orig Original message to copy.
     * @return A Message object from the global pool.
     */
    //static Message *obtain(Message *orig);

    /**
     * Same as {@link #obtain()}, but sets the value for the <em>target</em> member on the Message returned.
     * @param h  Handler to assign to the returned Message object's <em>target</em> member.
     * @return A Message object from the global pool.
     */
    static Message *obtain(MessageHandler *h);

    /**
     * Same as {@link #obtain(Handler)}, but assigns a callback Runnable on
     * the Message that is returned.
     * @param h  Handler to assign to the returned Message object's <em>target</em> member.
     * @param callback Runnable that will execute when the message is handled.
     * @return A Message object from the global pool.
     */
    //static Message *obtain(MessageHandler *h, Runnable *callback);

    /**
     * Same as {@link #obtain()}, but sets the values for both <em>target</em> and
     * <em>what</em> members on the Message.
     * @param h  Value to assign to the <em>target</em> member.
     * @param pWhat  Value to assign to the <em>what</em> member.
     * @return A Message object from the global pool.
     */
    static Message *obtain(MessageHandler *h, int pWhat);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, and <em>obj</em>
     * members.
     * @param h  The <em>target</em> value to set.
     * @param pWhat  The <em>what</em> value to set.
     * @param pObj  The <em>object</em> method to set.
     * @return  A Message object from the global pool.
     */
    static Message *obtain(MessageHandler *h, int pWhat, Object *pObj);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, 
     * <em>arg1</em>, and <em>arg2</em> members.
     * 
     * @param h  The <em>target</em> value to set.
     * @param pWhat  The <em>what</em> value to set.
     * @param pArg1  The <em>arg1</em> value to set.
     * @param arg2  The <em>arg2</em> value to set.
     * @return  A Message object from the global pool.
     */
    static Message *obtain(MessageHandler *h, int pWhat, int pArg1, int arg2);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, 
     * <em>arg1</em>, <em>arg2</em>, and <em>obj</em> members.
     * 
     * @param h  The <em>target</em> value to set.
     * @param pWhat  The <em>what</em> value to set.
     * @param pArg1  The <em>arg1</em> value to set.
     * @param arg2  The <em>arg2</em> value to set.
     * @param pObj  The <em>obj</em> value to set.
     * @return  A Message object from the global pool.
     */
    static Message *obtain(MessageHandler *h, int pWhat, int pArg1, int arg2, Object *pObj);

    /**
     * Return a Message instance to the global pool.  You MUST NOT touch
     * the Message after calling this function -- it has effectively been
     * freed.
     */
    void recycle();

    /**
     * Make this message like o.  Performs a shallow copy of the data field.
     * Does not copy the linked list fields, nor the timestamp or
     * target/callback of the original message.
     */
    void copyFrom(Message *o);

    /**
     * Return the targeted delivery time of this message, in milliseconds.
     */
    inline long getWhen() { return when; }
    
    inline void setTarget(MessageHandler *h) { target = h; }

    /**
     * Retrieve the a {@link android.os.Handler Handler} implementation that
     * will receive this message. The object must implement
     * {@link android.os.Handler#handleMessage(android.os.Message)
     * Handler.handleMessage()}. Each Handler has its own name-space for
     * message codes, so you do not need to
     * worry about yours conflicting with other handlers.
     */
    inline MessageHandler *getTarget() { return target; }

    /**
     * Retrieve callback object that will execute when this message is handled.
     * This object must implement Runnable. This is called by
     * the <em>target</em> {@link Handler} that is receiving this Message to
     * dispatch it.  If
     * not set, the message will be dispatched to the receiving Handler's
     * {@link Handler#handleMessage(Message Handler.handleMessage())}. */
    inline Runnable *getCallback() { return callback; }

    /**
     * Sends this Message to the Handler specified by {@link #getTarget}.
     * Throws a null pointer exception if this field has not been set.
     */
    void sendToTarget();

    void clearForRecycle();

    /** Constructor (but the preferred way to get a Message is to call {@link #obtain() Message.obtain()}).
    */
    Message();

    ~Message();

private:
    //static Object *mPoolSync;
    static Message *mPool;
    static int mPoolSize;
};

} // namespace IBox

#endif // __cplusplus

#endif // _Message_H_
