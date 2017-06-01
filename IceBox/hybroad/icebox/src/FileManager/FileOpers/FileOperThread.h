#ifndef __FileOperThread__H_
#define __FileOperThread__H_

#include "Thread.h"
#include "FileOper.h"
#include "MessageHandler.h"
#include "OperThreadStateListener.h"

namespace IBox {

class OperHandler : public MessageHandler {
public:
    OperHandler();
	~OperHandler();
	void handleMessage(Message* msg);
	bool isBusy() { return mIsBusy; }
	void setOperThreadStateListener(OperThreadStateListener* listener) { mThreadStateListener = listener; }
private:
	bool mIsBusy;
	OperThreadStateListener* mThreadStateListener;
};

class FileOperThread : public Thread {
public:
	FileOperThread();
	~FileOperThread();	
	void run();
	void execute(FileOper* oper);
	bool isBusy();
	void setOperThreadStateListener(OperThreadStateListener* listener); 
	
private:
	OperHandler* mOperHandler;
};

} /* namespace IBox */

#endif // __FileOperThread__H_

