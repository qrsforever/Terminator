#include "LogThread.h"

#include <unistd.h>

extern "C" void logInit();

namespace IBox {

LogThread::LogThread()
	: Thread()
{
}

LogThread::~LogThread()
{
}

void 
LogThread::run() 
{
    logInit();

    Thread::run();
}

LogThread* gLogThread = 0;

void LogThreadInit()
{
    if (!gLogThread) {
        gLogThread = new LogThread();
        gLogThread->start();
        usleep(200 * 1000);
    }
}

} /* namespace IBox */

