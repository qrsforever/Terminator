#include "FileExplorerAssertions.h"
#include "File.h"
#include "RecoverOper.h"
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
	
Recover::Recover(std::string user, std::string path)
	: mOperUser(user)
	, mTopPath(path)	
{
	
}

void Recover::accept(std::string path) 
{
	File file(path);
    if (file.isFile()) {
        if (mFileChangedListener != NULL)
            mFileChangedListener->onFileRecovered(mOperUser, file.path());
    } else if (file.isDirectory()) {
        file.listFiles(this);
        if (mFileChangedListener != NULL)
            mFileChangedListener->onDirRecovered(mOperUser, file.path());
    }
}

void Recover::start() 
{	
    accept(mTopPath);
}

RecoverOper::RecoverOper(int code, std::string request) 
	: FileOper(code, request)	
	, mResult(Result::OK)
{
	
}	

Result RecoverOper::execute() 
{
	JsonObject requestObj(mOperRequest);

	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	
	Recover re(operUser, path);
	re.setFileOperProgressListener(this);
	re.setFileChangedListener(FileDBManager::getInstance());
	re.start();	
	return Result::OK;
}

void RecoverOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	
	
} // namespace IBox



