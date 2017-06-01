
#include "MessageQueue.h"
#include "Message.h"
#include "SysTime.h"

#include <sys/select.h>


namespace IBox {

MessageQueue::MessageQueue()
	: mMessages(NULL)
	, mQuiting(false)
{
    pthread_cond_init(&mCond, NULL);
    pthread_mutex_init(&mMutex, NULL);
}

Message *
MessageQueue::next()
{
    pthread_mutex_lock(&mMutex);

    while (true) {
        uint32_t now;

        // Try to retrieve the next message, returning if found.
        now = SysTime::GetMSecs();
        Message *msg = pullNextLocked(now);
        if (msg != NULL) {
            pthread_mutex_unlock(&mMutex);
            return msg;
        }

        if (mMessages != NULL) {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 20 * 1000;
            pthread_mutex_unlock(&mMutex);
            select(0, NULL, NULL, NULL, &tv);
            pthread_mutex_lock(&mMutex);
        }
        else {
            pthread_cond_wait(&mCond, &mMutex);
        }
    }
}

Message *
MessageQueue::pullNextLocked(uint32_t now)
{
    Message *msg = mMessages;
    if (msg != NULL) {
        if (now >= msg->when) {
            mMessages = msg->next;
            return msg;
        }
    }

    return NULL;
}

bool 
MessageQueue::enqueueMessage(Message *msg, uint32_t when)
{
    pthread_mutex_lock(&mMutex);

    if (mQuiting) {
        pthread_mutex_unlock(&mMutex);
        return false;
    } else if (msg->target == NULL) {
        mQuiting = true;
    }

    msg->when = when;
    Message *p = mMessages;
    if (p == NULL || when == 0 || when < p->when) {
        msg->next = p;
        mMessages = msg;

        pthread_cond_signal(&mCond);
    } else {
        Message *prev = NULL;
        while (p != NULL && p->when <= when) {
            prev = p;
            p = p->next;
        }
        msg->next = prev->next;
        prev->next = msg;

        pthread_cond_signal(&mCond);
    }

    pthread_mutex_unlock(&mMutex);

    return true;
}

bool 
MessageQueue::removeMessages(MessageHandler *h, int what, Object *object, bool doRemove)
{
    pthread_mutex_lock(&mMutex);

    Message *p = mMessages;
    bool found = false;

    // Remove all messages at front.
    while (p != NULL && p->target == h && p->what == what
        && (object == NULL || p->obj == object)) {
        if (!doRemove) {
            pthread_mutex_unlock(&mMutex);
            return true;
        }
        found = true;
        Message *n = p->next;
        mMessages = n;
        p->recycle();
        p = n;
    }

    // Remove all messages after front.
    while (p != NULL) {
        Message *n = p->next;
        if (n != NULL) {
            if (n->target == h && n->what == what
                && (object == NULL || n->obj == object)) {
                if (!doRemove) {
                    pthread_mutex_unlock(&mMutex);
                    return true;
                }
                found = true;
                Message *nn = n->next;
                n->recycle();
                p->next = nn;
                continue;
            }
        }
        p = n;
    }

    pthread_mutex_unlock(&mMutex);
    
    return found;
}

bool 
MessageQueue::removeMessages(MessageHandler *h, int what, int arg1, int arg2, bool doRemove)
{
    pthread_mutex_lock(&mMutex);

    Message *p = mMessages;
    bool found = false;

    // Remove all messages at front.
    while (p != NULL && p->target == h && p->what == what && p->arg1 == arg1 && p->arg2 == arg2) {
        if (!doRemove) {
            pthread_mutex_unlock(&mMutex);
            return true;
        }
        found = true;
        Message *n = p->next;
        mMessages = n;
        p->recycle();
        p = n;
    }

    // Remove all messages after front.
    while (p != NULL) {
        Message *n = p->next;
        if (n != NULL) {
            if (n->target == h && n->what == what && n->arg1 == arg1 && n->arg2 == arg2) {
                if (!doRemove) {
                    pthread_mutex_unlock(&mMutex);
                    return true;
                }
                found = true;
                Message *nn = n->next;
                n->recycle();
                p->next = nn;
                continue;
            }
        }
        p = n;
    }

    pthread_mutex_unlock(&mMutex);
    
    return found;
}

void 
MessageQueue::removeMessages(MessageHandler *h, Runnable *r, Object *object)
{
    if (r == NULL) {
        return;
    }

    pthread_mutex_lock(&mMutex);

    Message *p = mMessages;

    // Remove all messages at front.
    while (p != NULL && p->target == h && p->callback == r
           && (object == NULL || p->obj == object)) {
        Message *n = p->next;
        mMessages = n;
        p->recycle();
        p = n;
    }

    // Remove all messages after front.
    while (p != NULL) {
        Message *n = p->next;
        if (n != NULL) {
            if (n->target == h && n->callback == r
            	&& (object == NULL || n->obj == object)) {
                Message *nn = n->next;
                n->recycle();
                p->next = nn;
                continue;
            }
        }
        p = n;
    }

    pthread_mutex_unlock(&mMutex);
}

void 
MessageQueue::removeCallbacksAndMessages(MessageHandler *h, Object *object)
{
    pthread_mutex_lock(&mMutex);

    Message *p = mMessages;

    // Remove all messages at front.
    while (p != NULL && p->target == h 
            && (object == NULL || p->obj == object)) {
        Message *n = p->next;
        mMessages = n;
        p->recycle();
        p = n;
    }

    // Remove all messages after front.
    while (p != NULL) {
        Message *n = p->next;
        if (n != NULL) {
            if (n->target == h && (object == NULL || n->obj == object)) {
                Message *nn = n->next;
                n->recycle();
                p->next = nn;
                continue;
            }
        }
        p = n;
    }

    pthread_mutex_unlock(&mMutex);
}

void 
MessageQueue::poke()
{
    pthread_mutex_lock(&mMutex);
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mMutex);
}

} // namespace IBox
