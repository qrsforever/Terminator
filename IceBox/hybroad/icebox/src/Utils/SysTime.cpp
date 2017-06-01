
#include "SysTime.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

namespace IBox {

void 
SysTime::GetDateTime(DateTime* dt)
{
    if (dt) {
        struct timeval current;
        struct tm temp_time;
        
        if (!gettimeofday(&current, NULL)){
            localtime_r(&current.tv_sec, &temp_time);
            dt->mYear       = temp_time.tm_year + 1900;
            dt->mMonth      = temp_time.tm_mon + 1;
            dt->mDayOfWeek  = temp_time.tm_wday;
            dt->mDay        = temp_time.tm_mday;
            dt->mHour       = temp_time.tm_hour;
            dt->mMinute     = temp_time.tm_min;
            dt->mSecond     = temp_time.tm_sec;
	    }
    }
}

uint32_t 
SysTime::GetMSecs()
{
    struct timespec tp;

    clock_gettime(CLOCK_MONOTONIC, &tp);
       
    return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}

} // namespace IBox
