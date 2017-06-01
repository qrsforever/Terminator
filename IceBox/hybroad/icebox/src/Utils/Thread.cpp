
#include "Thread.h"
#include "Message.h"
#include "MessageHandler.h"

#include <map>


namespace IBox {

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

static std::map<pthread_t, Thread *> gThreads;

Thread::Thread()
	: mMsgQueue()
	, mRunnable(NULL)
	//, nName(NULL)
{
    pthread_mutex_lock(&mMsgQueue.mMutex);

    pthread_create(&mID, NULL, threadEntry, this);

    pthread_mutex_lock(&gMutex);
    gThreads.insert(std::make_pair(mID, this));
    pthread_mutex_unlock(&gMutex);
}

Thread::Thread(Runnable *r)
	: mMsgQueue()
	, mRunnable(r)
	//, nName(NULL)
{
    pthread_mutex_lock(&mMsgQueue.mMutex);

    pthread_create(&mID, NULL, threadEntry, this);

    pthread_mutex_lock(&gMutex);
    gThreads.insert(std::make_pair(mID, this));
    pthread_mutex_unlock(&gMutex);
}

Thread::Thread(pthread_t id)
	: mMsgQueue()
	, mRunnable(NULL)
	//, nName(NULL)
{
    pthread_mutex_lock(&mMsgQueue.mMutex);

    mID = id;

    pthread_mutex_lock(&gMutex);
    gThreads.insert(std::make_pair(mID, this));
    pthread_mutex_unlock(&gMutex);
}

Thread::~Thread() {
    pthread_mutex_lock(&gMutex);
    std::map<pthread_t, Thread *>::iterator it;
    it = gThreads.find(mID);
    if (it == gThreads.end()) {
        ;
    }
    else {
        gThreads.erase(it);
    }
    pthread_mutex_unlock(&gMutex);
}

//void 
//Thread::setName(String *name) {
//    nName = name;
//}

void 
Thread::start() {
    pthread_mutex_unlock(&mMsgQueue.mMutex);
}

void 
Thread::run() {
    MessageQueue *queue = getMessageQueue();

    while (true) {
        Message *msg = queue->next(); // might block
        //if (!me.mRun) {
        //    break;
        //}
        if (msg != NULL) {
            if (msg->target == NULL) {
                // No target is a magic identifier for the quit message.
                return;
            }
            msg->target->dispatchMessage(msg);
            msg->recycle();
        }
    }
}

Thread *
Thread::currentThread() {
    Thread *curr = NULL;
    pthread_t tID;

    tID = pthread_self();

    pthread_mutex_lock(&gMutex);
    std::map<pthread_t, Thread *>::iterator it;
    it = gThreads.find(tID);
    if (it == gThreads.end()) {
        ;
    }
    else {
        curr = it->second;
    }
    pthread_mutex_unlock(&gMutex);
    return curr;
}

void *
Thread::threadEntry(void *r) {
    Thread *self = (Thread *)r;

    pthread_mutex_lock(&self->mMsgQueue.mMutex);
    pthread_mutex_unlock(&self->mMsgQueue.mMutex);

    if (self->mRunnable != NULL)
        self->mRunnable->run();
//    else
        self->run();

    return NULL;
}

} /* namespace IBox */
