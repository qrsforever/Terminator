#include "FileExplorerAssertions.h"
#include "FileExplorerThread.h"
#include "UserManager.h"
#include "DiskManager.h"
#include "FileOperHandler.h"
#include "BackgroundThread.h"
#include "FileOperThreadManager.h"

#include <unistd.h>
#include <stdio.h>

namespace IBox {

FileExplorerThread::FileExplorerThread() 
{
}

FileExplorerThread::~FileExplorerThread()
{
	
}

void FileExplorerThread::init()
{
	DiskManager::getInstance()->setNext(UserManager::getInstance());
	UserManager::getInstance()->setNext(FileOperHandler::getInstance());
}

void FileExplorerThread::sendMessage(int what, int arg1, int arg2, const char* data) 
{
	logMsg("what %d, arg1 %d, arg2 %d, data %s.\n", what, arg1, arg2, data);
	DiskManager::getInstance()->request(what, data);
}

static FileExplorerThread* gFileExplorerThread = NULL;

FileExplorerThread* FileExplorerThread::getInstance() 
{
    if (gFileExplorerThread == NULL)
        gFileExplorerThread = new FileExplorerThread();
    return gFileExplorerThread;
}

void FileExplorerThreadInit() 
{
	FileOperThreadManagerInit();
	FileExplorerThread::getInstance()->init();
	BackgroundThreadStart(); 
}

} // namespace IBox



