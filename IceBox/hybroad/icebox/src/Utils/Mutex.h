#ifndef __MUTEX__H
#define __MUTEX__H

#include <pthread.h>

#ifdef __cplusplus

// #define MUTEX_DEBUG 1

namespace IBox {

class Mutex {
public:
    Mutex(const char* objname = 0);
    ~Mutex();
    void lock();
    void unlock();
private:
#if MUTEX_DEBUG
    char mObjname[32];
#endif
    pthread_mutex_t _mutex;
};

}

#endif //__cplusplus

#endif
