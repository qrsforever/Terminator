#include "Mutex.h"

#if MUTEX_DEBUG
#include <stdio.h>
#include <string.h>
#endif

namespace IBox {

Mutex::Mutex(const char* objname)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&(_mutex),&attr);

#if MUTEX_DEBUG
    mObjname[0] = 0;
    if (objname)
        strncpy(mObjname, objname, 31);
    mObjname[31] = 0;
#endif
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&(_mutex));
}

void 
Mutex::lock()
{
#if MUTEX_DEBUG
    if (mObjname[0])
        printf("###DEBUG### lock %s\n", mObjname);
#endif
    pthread_mutex_lock(&(_mutex));
}

void 
Mutex::unlock()
{
#if MUTEX_DEBUG
    if (mObjname[0])
        printf("###DEBUG### unlock %s\n", mObjname);
#endif
    pthread_mutex_unlock(&(_mutex));
}

}
