#ifndef _MessageHandler_H_
#define _MessageHandler_H_

#include <stdint.h>

#include "Message.h"

#ifdef __cplusplus

namespace IBox {

class Object;
class Message;
class MessageQueue;
class Runnable;

class MessageHandler {
protected:
    /**
     * Subclasses must implement this to receive messages.
     */
    virtual void handleMessage(Message *msg) = 0;

public:
    /**
     * Callback interface you can use when instantiating a Handler to avoid
     * having to implement your own subclass of Handler.
     */
    class Callback {
        public: virtual ~Callback() {}
        public: virtual bool handleMessage(Message *msg) = 0;
    };
    
    /**
     * Handle system messages here.
     */
    virtual void dispatchMessage(Message *msg);

    /**
     * Default constructor associates this handler with the queue for the
     * current thread.
     *
     * If there isn't one, this handler won't be able to receive messages.
     */
    MessageHandler();

    /**
     * Constructor associates this handler with the queue for the
     * current thread and takes a callback interface in which you can handle
     * messages.
     */
    //MessageHandler(Callback *callback);

    //MessageHandler(Looper looper);
    //MessageHandler(Looper looper, Callback callback);

    virtual ~MessageHandler();

    Message *obtainMessage();
    Message *obtainMessage(int what);
    Message *obtainMessage(int what, Object *obj);
    Message *obtainMessage(int what, int arg1, int arg2);
    Message *obtainMessage(int what, int arg1, int arg2, Object *obj);

    bool post(Runnable *r);
    bool postAtTime(Runnable *r, uint32_t uptimeMillis);
    bool postAtTime(Runnable *r, Object *token, uint32_t uptimeMillis);
    bool postDelayed(Runnable *r, uint32_t delayMillis);
    bool postAtFrontOfQueue(Runnable *r);

    void removeCallbacks(Runnable *r);
    void removeCallbacks(Runnable *r, Object *token);

    bool sendMessage(Message *msg);
    bool sendEmptyMessage(int what);
    bool sendEmptyMessageDelayed(int what, uint32_t delayMillis);
    bool sendEmptyMessageAtTime(int what, uint32_t uptimeMillis);
    bool sendMessageDelayed(Message *msg, uint32_t delayMillis);
    bool sendMessageAtTime(Message *msg, uint32_t uptimeMillis);
    bool sendMessageAtFrontOfQueue(Message *msg);

    void removeMessages(int what);
    void removeMessages(int what, Object *object);
    void removeMessages(int what, int arg1, int arg2);
    void removeCallbacksAndMessages(Object *token);

    bool hasMessages(int what);
    bool hasMessages(int what, Object *object);

    Message *getPostMessage(Runnable *r);
    Message *getPostMessage(Runnable *r, Object *token);

    void handleCallback(Message *message);

    MessageQueue *mQueue;
    Callback *mCallback;
};

} // namespace IBox

#endif // __cplusplus

#endif // _MessageHandler_H_
