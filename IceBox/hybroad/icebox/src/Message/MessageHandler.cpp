
#include "MessageHandler.h"
#include "Message.h"
#include "Thread.h"
#include "SysTime.h"


namespace IBox {

/**
 * Handle system messages here.
 */
void 
MessageHandler::dispatchMessage(Message *msg)
{
    if (msg->callback != NULL) {
        handleCallback(msg);
    }
    else {
        if (mCallback != NULL) {
            if (mCallback->handleMessage(msg)) {

                return;
            }
        }
        handleMessage(msg);
    }
}

/**
 * Default constructor associates this handler with the queue for the
 * current thread.
 *
 * If there isn't one, this handler won't be able to receive messages.
 */
MessageHandler::MessageHandler()
{
    Thread *curr = Thread::currentThread();
    mQueue = curr->getMessageQueue();
    mCallback = NULL;
}

MessageHandler::~MessageHandler()
{
    removeCallbacksAndMessages(NULL);
}

/**
 * Returns a new {@link android.os.Message Message} from the global message pool. More efficient than
 * creating and allocating new instances. The retrieved message has its handler set to this instance (Message.target == this).
 *  If you don't want that facility, just call Message.obtain() instead.
 */
Message *
MessageHandler::obtainMessage()
{
    return Message::obtain(this);
}

/**
 * Same as {@link #obtainMessage()}, except that it also sets the what member of the returned Message.
 * 
 * @param what Value to assign to the returned Message.what field.
 * @return A Message from the global message pool.
 */
Message *
MessageHandler::obtainMessage(int what)
{
    return Message::obtain(this, what);
}

/**
 * Same as {@link #obtainMessage()}, except that it also sets the what and obj members 
 * of the returned Message.
 * 
 * @param what Value to assign to the returned Message.what field.
 * @param obj Value to assign to the returned Message.obj field.
 * @return A Message from the global message pool.
 */
Message *
MessageHandler::obtainMessage(int what, Object *obj)
{
    return Message::obtain(this, what, obj);
}

/**
 * Same as {@link #obtainMessage()}, except that it also sets the what, arg1 and arg2 members of the returned
 * Message.
 * @param what Value to assign to the returned Message.what field.
 * @param arg1 Value to assign to the returned Message.arg1 field.
 * @param arg2 Value to assign to the returned Message.arg2 field.
 * @return A Message from the global message pool.
 */
Message *
MessageHandler::obtainMessage(int what, int arg1, int arg2)
{
    return Message::obtain(this, what, arg1, arg2);
}

/**
 * Same as {@link #obtainMessage()}, except that it also sets the what, obj, arg1,and arg2 values on the 
 * returned Message.
 * @param what Value to assign to the returned Message.what field.
 * @param arg1 Value to assign to the returned Message.arg1 field.
 * @param arg2 Value to assign to the returned Message.arg2 field.
 * @param obj Value to assign to the returned Message.obj field.
 * @return A Message from the global message pool.
 */
Message *
MessageHandler::obtainMessage(int what, int arg1, int arg2, Object *obj)
{
    return Message::obtain(this, what, arg1, arg2, obj);
}

/**
 * Causes the Runnable r to be added to the message queue.
 * The runnable will be run on the thread to which this handler is 
 * attached. 
 *  
 * @param r The Runnable that will be executed.
 * 
 * @return Returns true if the Runnable was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::post(Runnable *r)
{
    return sendMessageDelayed(getPostMessage(r), 0);
}

/**
 * Causes the Runnable r to be added to the message queue, to be run
 * at a specific time given by <var>uptimeMillis</var>.
 * <b>The time-base is {@link android.os.SystemClock#uptimeMillis}.</b>
 * The runnable will be run on the thread to which this handler is attached.
 *
 * @param r The Runnable that will be executed.
 * @param uptimeMillis The absolute time at which the callback should run,
 *         using the {@link android.os.SystemClock#uptimeMillis} time-base.
 *  
 * @return Returns true if the Runnable was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.  Note that a
 *         result of true does not mean the Runnable will be processed -- if
 *         the looper is quit before the delivery time of the message
 *         occurs then the message will be dropped.
 */
bool 
MessageHandler::postAtTime(Runnable *r, uint32_t uptimeMillis)
{
    return sendMessageAtTime(getPostMessage(r), uptimeMillis);
}

/**
 * Causes the Runnable r to be added to the message queue, to be run
 * at a specific time given by <var>uptimeMillis</var>.
 * <b>The time-base is {@link android.os.SystemClock#uptimeMillis}.</b>
 * The runnable will be run on the thread to which this handler is attached.
 *
 * @param r The Runnable that will be executed.
 * @param uptimeMillis The absolute time at which the callback should run,
 *         using the {@link android.os.SystemClock#uptimeMillis} time-base.
 * 
 * @return Returns true if the Runnable was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.  Note that a
 *         result of true does not mean the Runnable will be processed -- if
 *         the looper is quit before the delivery time of the message
 *         occurs then the message will be dropped.
 *         
 * @see android.os.SystemClock#uptimeMillis
 */
bool 
MessageHandler::postAtTime(Runnable *r, Object *token, uint32_t uptimeMillis)
{
    return sendMessageAtTime(getPostMessage(r, token), uptimeMillis);
}

/**
 * Causes the Runnable r to be added to the message queue, to be run
 * after the specified amount of time elapses.
 * The runnable will be run on the thread to which this handler
 * is attached.
 *  
 * @param r The Runnable that will be executed.
 * @param delayMillis The delay (in milliseconds) until the Runnable
 *        will be executed.
 *        
 * @return Returns true if the Runnable was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.  Note that a
 *         result of true does not mean the Runnable will be processed --
 *         if the looper is quit before the delivery time of the message
 *         occurs then the message will be dropped.
 */
bool 
MessageHandler::postDelayed(Runnable *r, uint32_t delayMillis)
{
    return sendMessageDelayed(getPostMessage(r), delayMillis);
}

/**
 * Posts a message to an object that implements Runnable.
 * Causes the Runnable r to executed on the next iteration through the
 * message queue. The runnable will be run on the thread to which this
 * handler is attached.
 * <b>This method is only for use in very special circumstances -- it
 * can easily starve the message queue, cause ordering problems, or have
 * other unexpected side-effects.</b>
 *  
 * @param r The Runnable that will be executed.
 * 
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::postAtFrontOfQueue(Runnable *r)
{
    return sendMessageAtFrontOfQueue(getPostMessage(r));
}

/**
 * Remove any pending posts of Runnable r that are in the message queue.
 */
void 
MessageHandler::removeCallbacks(Runnable *r)
{
    mQueue->removeMessages(this, r, NULL);
}

/**
 * Remove any pending posts of Runnable <var>r</var> with Object
 * <var>token</var> that are in the message queue.
 */
void 
MessageHandler::removeCallbacks(Runnable *r, Object *token)
{
    mQueue->removeMessages(this, r, token);
}

/**
 * Pushes a message onto the end of the message queue after all pending messages
 * before the current time. It will be received in {@link #handleMessage},
 * in the thread attached to this handler.
 *  
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::sendMessage(Message *msg)
{
    return sendMessageDelayed(msg, 0);
}

/**
 * Sends a Message containing only the what value.
 *  
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::sendEmptyMessage(int what)
{
    return sendEmptyMessageDelayed(what, 0);
}

/**
 * Sends a Message containing only the what value, to be delivered
 * after the specified amount of time elapses.
 * @see #sendMessageDelayed(android.os.Message, long) 
 * 
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::sendEmptyMessageDelayed(int what, uint32_t delayMillis)
{
    Message *msg = Message::obtain();
    msg->what = what;
    return sendMessageDelayed(msg, delayMillis);
}

/**
 * Sends a Message containing only the what value, to be delivered 
 * at a specific time.
 * @see #sendMessageAtTime(android.os.Message, long)
 *  
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */

bool 
MessageHandler::sendEmptyMessageAtTime(int what, uint32_t uptimeMillis)
{
    Message *msg = Message::obtain();
    msg->what = what;
    return sendMessageAtTime(msg, uptimeMillis);
}

/**
 * Enqueue a message into the message queue after all pending messages
 * before (current time + delayMillis). You will receive it in
 * {@link #handleMessage}, in the thread attached to this handler.
 *  
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.  Note that a
 *         result of true does not mean the message will be processed -- if
 *         the looper is quit before the delivery time of the message
 *         occurs then the message will be dropped.
 */
bool 
MessageHandler::sendMessageDelayed(Message *msg, uint32_t delayMillis)
{
    //parameter delayMillis is never less than 0;
    /*if (delayMillis < 0) {
        delayMillis = 0;
    }*/
    return sendMessageAtTime(msg, SysTime::GetMSecs() + delayMillis);
}

/**
 * Enqueue a message into the message queue after all pending messages
 * before the absolute time (in milliseconds) <var>uptimeMillis</var>.
 * <b>The time-base is {@link android.os.SystemClock#uptimeMillis}.</b>
 * You will receive it in {@link #handleMessage}, in the thread attached
 * to this handler.
 * 
 * @param uptimeMillis The absolute time at which the message should be
 *         delivered, using the
 *         {@link android.os.SystemClock#uptimeMillis} time-base.
 *         
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.  Note that a
 *         result of true does not mean the message will be processed -- if
 *         the looper is quit before the delivery time of the message
 *         occurs then the message will be dropped.
 */
bool 
MessageHandler::sendMessageAtTime(Message *msg, uint32_t uptimeMillis)
{
    bool sent = false;
    MessageQueue *queue = mQueue;
    if (queue != NULL) {
        msg->target = this;
        sent = queue->enqueueMessage(msg, uptimeMillis);
    }
    else {
        //RuntimeException e = new RuntimeException(
        //    this + " sendMessageAtTime() called with no mQueue");
        //Log.w("Looper", e.getMessage(), e);
    }
    return sent;
}

/**
 * Enqueue a message at the front of the message queue, to be processed on
 * the next iteration of the message loop.  You will receive it in
 * {@link #handleMessage}, in the thread attached to this handler.
 * <b>This method is only for use in very special circumstances -- it
 * can easily starve the message queue, cause ordering problems, or have
 * other unexpected side-effects.</b>
 *  
 * @return Returns true if the message was successfully placed in to the 
 *         message queue.  Returns false on failure, usually because the
 *         looper processing the message queue is exiting.
 */
bool 
MessageHandler::sendMessageAtFrontOfQueue(Message *msg)
{
    bool sent = false;
    MessageQueue *queue = mQueue;
    if (queue != NULL) {
        msg->target = this;
        sent = queue->enqueueMessage(msg, 0);
    }
    else {
        //RuntimeException e = new RuntimeException(
        //    this + " sendMessageAtTime() called with no mQueue");
        //Log.w("Looper", e.getMessage(), e);
    }
    return sent;
}

/**
 * Remove any pending posts of messages with code 'what' that are in the
 * message queue.
 */
void 
MessageHandler::removeMessages(int what)
{
    mQueue->removeMessages(this, what, NULL, true);
}

/**
 * Remove any pending posts of messages with code 'what and arg1 and arg2' that are in the
 * message queue.
 */
void 
MessageHandler::removeMessages(int what, int arg1, int arg2)
{
    mQueue->removeMessages(this, what, arg1, arg2, true);
}

/**
 * Remove any pending posts of messages with code 'what' and whose obj is
 * 'object' that are in the message queue.
 */
void 
MessageHandler::removeMessages(int what, Object *object)
{
    mQueue->removeMessages(this, what, object, true);
}

/**
 * Remove any pending posts of callbacks and sent messages whose
 * <var>obj</var> is <var>token</var>.
 */
void 
MessageHandler::removeCallbacksAndMessages(Object *token)
{
    mQueue->removeCallbacksAndMessages(this, token);
}

/**
 * Check if there are any pending posts of messages with code 'what' in
 * the message queue.
 */
bool 
MessageHandler::hasMessages(int what)
{
    return mQueue->removeMessages(this, what, NULL, false);
}

/**
 * Check if there are any pending posts of messages with code 'what' and
 * whose obj is 'object' in the message queue.
 */
bool 
MessageHandler::hasMessages(int what, Object *object)
{
    return mQueue->removeMessages(this, what, object, false);
}

Message *
MessageHandler::getPostMessage(Runnable *r)
{
    Message *m = Message::obtain();
    m->callback = r;
    m->callback->safeRef();
    return m;
}

Message *
MessageHandler::getPostMessage(Runnable *r, Object *token)
{
    Message *m = Message::obtain();
    m->obj = token;
    m->obj->safeRef();
    m->callback = r;
    m->callback->safeRef();
    return m;
}

void 
MessageHandler::handleCallback(Message *message)
{
    message->callback->run();
}

} // namespace IBox
