#include "Assertions.h"

#include "Log.h"
#include "RingBuffer.h"
#include "LogPool.h"
#include "LogPrinter.h"
#include "LogModule.h"
#include "SysTime.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOG_BUFFER_SIZE	128 * 1024

static int s_SystemModuleFlag = 0;
int gSystemModuleLevel = LOG_LEVEL_WARNING;
static LogModule LogSysOperModule("logSys", s_SystemModuleFlag, gSystemModuleLevel);

static int s_RunModuleFlag = 0;
int gRunModuleLevel = LOG_LEVEL_WARNING;
static LogModule LogRunOperModule("logRun", s_RunModuleFlag, gRunModuleLevel);

static int s_SecureModuleFlag = 0;
int gSecureModuleLevel = LOG_LEVEL_WARNING;
static LogModule LogSecureOperModule("logSecure", s_SecureModuleFlag, gSecureModuleLevel);

static int s_UserModuleFlag = 0;
int gUserModuleLevel = LOG_LEVEL_WARNING;
static LogModule LogUserOperModule("logUser", s_UserModuleFlag, gUserModuleLevel);


static uint8_t* g_logBuffer = NULL;
static IBox::RingBuffer* g_ringBuffer = NULL;

static IBox::Log g_log;

IBox::LogPool* g_logPool = NULL;
IBox::LogPrinter* g_logPrinter = NULL;

extern "C" 
void logModuleInit()
{
    setModuleFlag("logSys", HLG_OPERATION);
    setModuleFlag("logRun" , HLG_RUNNING);
    setModuleFlag("logSecure", HLG_SECURITY);
    setModuleFlag("logUser", HLG_USER);
}

extern "C" 
void logInit()
{
    g_logBuffer = (uint8_t*)malloc(LOG_BUFFER_SIZE);
    g_ringBuffer = new IBox::RingBuffer(g_logBuffer, LOG_BUFFER_SIZE);

    g_logPool = new IBox::LogPool();
    g_logPool->setBuffer(g_ringBuffer);

    g_logPrinter = new IBox::LogPrinter();
    g_logPool->attachFilter(g_logPrinter, 0);

    g_log.setBuffer(g_ringBuffer);
    g_log.attachSink(g_logPool);

    logModuleInit();

#if defined(SET_LOGLEVEL_FROM_FILE)
    SetLogLevelFromFile("/root/debug.ini");
#endif
}

extern "C" 
void logSetExtensionStyle(int value)
{
    g_log.setExtensionStyle(value);
}

extern "C" 
void logCStyle(int level, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    g_log.log(level, fmt, args);
    va_end(args);
}

extern "C" 
void logVerboseCStyle(const char* file, int line, const char* function, LogType pType, int level, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    g_log.logVerbose(file, line, function, pType, level, fmt, args);
    va_end(args);
}
