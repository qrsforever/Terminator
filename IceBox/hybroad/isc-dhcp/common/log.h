#ifndef __LOG__H_
#define __LOG__H_

extern void logVerboseCStyle(const char* file, int line, const char* function, int pType, int level, const char* fmt, ...);
#define DHCP_LOG_INFO(args...) \
do { \
    logVerboseCStyle(__FILE__, __LINE__, __FUNCTION__, 0, 3, args); \
} while(0)

#define DHCP_LOG_WARN(args...) \
do { \
    logVerboseCStyle(__FILE__, __LINE__, __FUNCTION__, 0, 2, args); \
} while(0)

#define DHCP_LOG_ERROR(args...) \
do { \
    logVerboseCStyle(__FILE__, __LINE__, __FUNCTION__, 0, 1, args); \
} while(0)

#define DHCP_LOG_FATAL(args...) \
do { \
    logVerboseCStyle(__FILE__, __LINE__, __FUNCTION__, 0, 0, args); \
} while(0)

#endif
