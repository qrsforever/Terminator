#include "FileExplorerAssertions.h"
#include "Message.h"
#include "FileOperThread.h"

namespace IBox {

OperHandler::OperHandler()
    : mIsBusy(false)	
    , mThreadStateListener(NULL) 	
{
}

OperHandler::~OperHandler()
{
}

void OperHandler::handleMessage(Message* msg)
{
	logMsg("Thread [%p] handleMessage.\n", pthread_self());
	mIsBusy = true;
	if (mThreadStateListener)
		mThreadStateListener->onBusy();
	if (msg) {
		FileOper* oper = dynamic_cast<FileOper*>(msg->obj);
		if (oper) oper->start();
	}
	mIsBusy = false;
	if (mThreadStateListener)
		mThreadStateListener->onIdle();
}

FileOperThread::FileOperThread() 
	: mOperHandler(NULL) 
{
}

FileOperThread::~FileOperThread() 
{
	if (mOperHandler != NULL) 
		delete mOperHandler;
}

void 
FileOperThread::run() 
{
	logMsg("FileOperThread [%p] is running.\n", pthread_self());
	if (mOperHandler == NULL) 
		mOperHandler = new OperHandler();
    Thread::run();
}

void 
FileOperThread::execute(FileOper* oper)
{
	if (oper) {
		Message* m = mOperHandler->obtainMessage(0, 0, 0, oper);
		mOperHandler->sendMessage(m);
		oper->safeUnref();
	}	
}

bool
FileOperThread::isBusy()
{
	if (mOperHandler)	
		return mOperHandler->isBusy();
	return true;	
}

void FileOperThread::setOperThreadStateListener(OperThreadStateListener* listener) 
{ 
	if (mOperHandler)
		mOperHandler->setOperThreadStateListener(listener);
}
	
} // namespace IBox


