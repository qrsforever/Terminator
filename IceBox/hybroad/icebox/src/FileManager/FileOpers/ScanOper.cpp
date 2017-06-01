#include "FileExplorerAssertions.h"
#include "File.h"
#include "ScanOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "UserManager.h"
#include "MessageTypes.h"
#include "FileDBManager.h"
#include "NativeHandler.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{
	
Scan::Scan(std::string username, std::string topDir) 
	: mOperUser(username)
	, mTopDir(topDir)	
	, mFileNumber(0)
	, mFolderNumber(0)
	, mTotalFileSize(0)
{

}

void Scan::accept(std::string path) 
{
	if (mPermissionManager 
		&& !mPermissionManager->checkAccess(mOperUser, path))
		return ;

	if (mFileNameFilter && mFileNameFilter->check(path)) 
		return ;	
	
	File file(path);
	
	if (file.isFile()) {
		mFileNumber ++;
        mTotalFileSize += file.size();
        
        if (mFileChangedListener != NULL)
            mFileChangedListener->onFileCreated(mOperUser, file.path());

        if (mFileChangedListener != NULL) 
            mFileChangedListener->onSizeChanged(file.getParent());

    } else if (file.isDirectory()) {
    	if (mFileChangedListener != NULL)
            mFileChangedListener->onDirCreated(mOperUser, file.path());
        mFolderNumber ++;    
        file.listFiles(this);
		if (mFileChangedListener != NULL)
            mFileChangedListener->onSizeChanged(file.path());
    }
}

void Scan::start() 
{	
    accept(mTopDir);
}

ScanOper::ScanOper(int code, std::string request) 
	: FileOper(code, request)
{
	JsonObject requestObj(mOperRequest);
	mPath = requestObj.getString("Path");
	mOperUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (mOperUser.empty()) 
		mOperUser = "public";
}	

ScanOper::ScanOper(std::string path) 
	: FileOper(0, "")
	, mPath(path)
	, mOperUser("public")
{
	
}	

Result ScanOper::execute() 
{	
	ExcludeFilesFilter filter;
    filter.add("*.hyicon");
	filter.add("*.thumbnail");
	filter.add("*.partitions");
    filter.add("*/icebox/tmp");
    filter.add("*/icebox/backup");
	filter.add("*/icebox/icebox.db");
    
	Scan scan(mOperUser, mPath);
	scan.setFileOperProgressListener(this);
	scan.setFileNameFilter(&filter);
	scan.setFileChangedListener(FileDBManager::getInstance());
	scan.start();
	
	return Result::OK;
}

void ScanOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	

ScanDisk::ScanDisk(std::string path) 
	: ScanOper(path)
{
	
}	

void ScanDisk::onFinished(Result result)
{
	logMsg("onFinished.\n");
	//NativeHandler& H = defNativeHandler();
	//H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_RECEIVE, 0));	
}

} // namespace IBox



