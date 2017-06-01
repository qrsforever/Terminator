#ifndef __BackgroundThread__H_
#define __BackgroundThread__H_

#include "Thread.h"
#include "FileFilter.h"
#include "ExcludeFilesFilter.h"
#include "FileChangedListener.h"

#include <list>
#include <string>

namespace IBox {

class BackgroundThread : public Thread { 
public:
    static BackgroundThread* getInstance(); 
	void run();
	
private: 
	bool diskDeviceIsEnough();
	bool isIdleState();
    BackgroundThread();
	~BackgroundThread();
};

void BackgroundThreadStart();

} /* namespace IBox */

#endif // __BackgroundThread__H_

