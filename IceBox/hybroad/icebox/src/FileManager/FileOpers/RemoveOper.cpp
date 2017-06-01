#include "FileExplorerAssertions.h"
#include "MessageTypes.h"
#include "File.h"
#include "RemoveOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{

Remove::Remove(std::string user, std::string path)
	: mOperUser(user)
	, mTopPath(path)	
{
	
}

void Remove::accept(std::string path) 
{
	File file(path);
    if (file.isFile()) {
        if (mFileChangedListener != NULL)
            mFileChangedListener->onFileDeleted(mOperUser, file.path());
    } else if (file.isDirectory()) {
        file.listFiles(this);
        if (mFileChangedListener != NULL)
            mFileChangedListener->onDirDeleted(mOperUser, file.path());
    }
}

void Remove::start() 
{
    accept(mTopPath);
}

RemoveOper::RemoveOper(int code, std::string request) 
	: FileOper(code, request)	
	, mResult(Result::OK)
{
	
}	

Result RemoveOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	
    Remove rm(operUser, path);
	//mv.setFileNameFilter(&filter);
	//mv.setPermissionChecker(FileDBManager::getInstance());
	rm.setFileChangedListener(FileDBManager::getInstance());
	//mv.setFileOperProgressListener(this);
	rm.start();
	return Result::OK;
}

void RemoveOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	
	
} // namespace IBox


