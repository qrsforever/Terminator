#include "MainThread.h"
#include "NativeHandler.h"

namespace IBox {

IBox::MainThread *gMainThread = NULL;

MainThread::MainThread()
	: Thread(pthread_self())
{
}

MainThread::~MainThread()
{
}

void MainThreadInit()
{
    if (!gMainThread) {
        gMainThread = new IBox::MainThread();
        gMainThread->start();
    }
}

void MainThreadRun()
{
    if (gMainThread) {
        defNativeHandler().setState(NativeHandler::kBoot);
        gMainThread->run();
    }
}

} /* namespace IBox */

