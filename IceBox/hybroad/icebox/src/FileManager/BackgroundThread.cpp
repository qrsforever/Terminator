#include "FileExplorerAssertions.h"
#include "BackgroundThread.h"
#include "DiskManager.h"
#include "UserManager.h"
#include "BackupOper.h"
#include "CalculateHashCodeOper.h"

#include <unistd.h>


namespace IBox {

BackgroundThread::BackgroundThread() 
{
	
}

BackgroundThread::~BackgroundThread() 
{	
	
}

bool BackgroundThread::diskDeviceIsEnough()
{
	return (DiskManager::getInstance()->localDiskDeviceCount() >= 2);
}

bool BackgroundThread::isIdleState()
{
	return (UserManager::getInstance()->getOnlineUserNumber() == 0);
}
	
void 
BackgroundThread::run() 
{   
	logMsg("Background Thread Running!\n");
	
	while (true) {	
		sleep(300);
		if (!isIdleState()) {
			logMsg("Not idle state.\n");
			continue;
		}

		if (diskDeviceIsEnough()) {
			BackupOper::backup();
			continue;
		}

		CalculateHashCodeOper::calculate();
	}
}

static BackgroundThread* gBackgroundThread = NULL;

BackgroundThread* 
BackgroundThread::getInstance() 
{
    if (gBackgroundThread == NULL)
        gBackgroundThread = new BackgroundThread();
    return gBackgroundThread;
}

void BackgroundThreadStart() 
{
    BackgroundThread::getInstance()->start();
}

} // namespace IBox


