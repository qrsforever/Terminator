#include "FileExplorerAssertions.h"
#include "FileOperThreadManager.h"
#include "FileOper.h"

#include <unistd.h>
#include <stdio.h>

namespace IBox {

static const int Max_Thread_Count = 2;

FileOperThreadManager::FileOperThreadManager() 
{
	for (int i = 0; i < Max_Thread_Count; i++) {
		FileOperThread* t = new FileOperThread();
		t->start();
		mThreads.push_back(t);
	}
}

FileOperThreadManager::~FileOperThreadManager()
{
	std::vector<FileOperThread*>::iterator it;
	it = mThreads.begin();
	while (it != mThreads.end()) {
		delete *it;
		it = mThreads.erase(it);	
	}
}

void FileOperThreadManager::execute(FileOper* oper)
{
	std::vector<FileOperThread*>::iterator it;
	
	for (it = mThreads.begin(); it != mThreads.end(); it++) {
		if (!(*it)->isBusy()) {
			(*it)->setOperThreadStateListener(this);
			(*it)->execute(oper);
			return ;
		}
	}
	mFileOpers.push_back(oper);
}

void FileOperThreadManager::onBusy()
{	
}

void FileOperThreadManager::onIdle()
{
	logMsg("onFileOperFinished\n");
	std::vector<FileOperThread*>::iterator it;
	it = mThreads.begin();
	while (it != mThreads.end()) {
		if (!(*it)->isBusy() && mFileOpers.size() > 0) {
			std::vector<FileOper*>::iterator iter;
			iter = mFileOpers.begin();
			(*it)->setOperThreadStateListener(this);
			(*it)->execute(*iter);
			mFileOpers.erase(iter);	
			return ;
		}
		it++;	
	}	
}
	
static FileOperThreadManager* gFileOperThreadManager = NULL;

FileOperThreadManager* 
FileOperThreadManager::getInstance() 
{
    if (gFileOperThreadManager == NULL)
        gFileOperThreadManager = new FileOperThreadManager();
    return gFileOperThreadManager;
}

void FileOperThreadManagerInit() 
{
	FileOperThreadManager::getInstance();
}

} // namespace IBox



