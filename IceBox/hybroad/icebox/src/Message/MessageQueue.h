#ifndef _MessageQueue_H_
#define _MessageQueue_H_

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus

namespace IBox {

class Object;
class Message;
class MessageHandler;
class Runnable;

class MessageQueue {
public:
    MessageQueue();

    Message *next();
    Message *pullNextLocked(uint32_t now);
    bool enqueueMessage(Message *msg, uint32_t when);
    bool removeMessages(MessageHandler *h, int what, Object *object, bool doRemove);
    bool removeMessages(MessageHandler *h, int what, int arg1, int arg2, bool doRemove);
    void removeMessages(MessageHandler *h, Runnable *r, Object *object);
    void removeCallbacksAndMessages(MessageHandler *h, Object *object);
    void poke();

    pthread_cond_t mCond;
    pthread_mutex_t mMutex;
private:
    Message *mMessages;
    bool mQuiting;
};

} // namespace IBox

#endif // __cplusplus

#endif // _MessageQueue_H_
