#ifndef _Log_H_
#define _Log_H_

#include "DataSource.h"
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include "Assertions.h"

#ifdef __cplusplus

namespace IBox {
class Log : public DataSource {
public:
    Log();
    ~Log();

    void setExtensionStyle(int value) { mExtStyle = value; }
    int  getExtensionStyle(void) { return mExtStyle; }
    void log(int level, const char* fmt, va_list args);
    void logVerbose(const char* file, int line, const char* function, LogType pType, int level, const char* fmt, va_list args);
protected:
    int logPrefix(char* buffer, int length, const char* file, int line, const char* function, LogType pType, int level);

private:
    int mExtStyle;
};

} // namespace IBox

#endif // __cplusplus
#endif // _Log_H_
