#include "FileExplorerAssertions.h"
#include "MessageTypes.h"
#include "File.h"
#include "CleanOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <stdio.h>
#include <unistd.h>
#include <vector>

namespace IBox
{

Clean::Clean(std::string user, std::string path)
	: mOperUser(user)
	, mTopPath(path)
{
	
}

void Clean::accept(std::string path) 
{
	File file(path);
    if (file.isFile()) {
		if (mFileChangedListener
			&& mPermissionManager 
			&& mPermissionManager->checkDelete(mOperUser, path)
			&& file.remove()) {
    		mFileChangedListener->onFileDeleted(mOperUser, file.path());
		}	
    } else if (file.isDirectory()) {
        file.listFiles(this);
		if (mFileChangedListener
			&& mPermissionManager 
			&& mPermissionManager->checkDelete(mOperUser, path)
			&& rmdir(path.c_str()) == 0) {
    		mFileChangedListener->onDirDeleted(mOperUser, file.path());
		}
    }
}

void Clean::start() 
{
    accept(mTopPath);
}

CleanOper::CleanOper(int code, std::string request) 
	: FileOper(code, request)	
{
	
}	

Result CleanOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	
    Clean rm(operUser, path);
	rm.setPermissionManager(FileDBManager::getInstance());
	rm.setFileChangedListener(FileDBManager::getInstance());
	rm.setFileOperProgressListener(this);
	rm.start();
	return Result::OK;
}	

CleanTrashOper::CleanTrashOper(int code, std::string request) 
	: FileOper(code, request)	
{
	
}	

Result CleanTrashOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";

	std::vector<std::string> deletedFiles;
	FileDBManager::getInstance()->listDeletedFiles(operUser, deletedFiles);
	for (int i = 0; i < deletedFiles.size(); i++) {
	    Clean rm(operUser, deletedFiles[i]);
		rm.setPermissionManager(FileDBManager::getInstance());
		rm.setFileChangedListener(FileDBManager::getInstance());
		rm.setFileOperProgressListener(this);
		rm.start();
	}
	return Result::OK;
}
	
} // namespace IBox



