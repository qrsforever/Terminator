#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

void logVerboseCStyle(const char* file, int line, const char* function, int pType, int level, const char* fmt, ...)
{
    static const char* textLevel[] = {"Assert : ", "Error! : ", "Warning: ", "Normal : ", "Verbose: "};
    static char logbuff[4096] = { 0 };
    struct timeval current;
    struct timespec specs;
    struct tm dt;
    va_list args;
    gettimeofday(&current, NULL);
    localtime_r(&current.tv_sec, &dt);
    clock_gettime(CLOCK_MONOTONIC, &specs);
    va_start(args, fmt);
    vsnprintf(logbuff, 4088, fmt, args);
    va_end(args);
    fprintf(stderr, "%02d:%02d:%02d.%03d | %s:%d | %s %s %s", dt.tm_hour, dt.tm_min, dt.tm_sec, specs.tv_nsec / 1000000, strrchr(file, '/') + 1, line, function, textLevel[level], logbuff);
    return;
}
