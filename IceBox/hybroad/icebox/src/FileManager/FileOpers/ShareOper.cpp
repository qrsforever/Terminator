#include "FileExplorerAssertions.h"
#include "File.h"
#include "ShareOper.h"
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
	
ShareFile::ShareFile(std::string user, std::string toUser, std::string path, bool share)
	: mOperUser(user)
	, mToUser(toUser)
	, mTopPath(path)
	, mShare(share)	
{
	
}

void ShareFile::accept(std::string path) 
{
    File file(path);
	if (file.isDirectory())
        file.listFiles(this);
    if (mFileChangedListener != NULL)
    	mFileChangedListener->onShareChanged(mOperUser, mToUser, file.path(), mShare);
}

void ShareFile::start() 
{	
    accept(mTopPath);
}

ShareOper::ShareOper(int code, std::string request) 
	: FileOper(code, request)	
	, mResult(Result::OK)
{
	
}	

Result ShareOper::execute() 
{
	JsonObject requestObj(mOperRequest);

	bool bShare = true;
	std::string cmd = requestObj.getString("Command");
	if (cmd.compare("Unshare") == 0)
		bShare = false;
	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	std::string toUser = requestObj.getString("ToUser");
	
	ShareFile share(operUser, toUser, path, bShare);
	share.setFileOperProgressListener(this);
	share.setFileChangedListener(FileDBManager::getInstance());
	share.start();	
	return Result::OK;
}

void ShareOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	
	
} // namespace IBox



