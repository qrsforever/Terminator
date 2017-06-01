#include "FileExplorerAssertions.h"
#include "File.h"
#include "MessageTypes.h"
#include "ChownOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{

Chown::Chown(std::string operUser, std::string toUser, std::string path)
	: mOperUser(operUser)
	, mToUser(toUser)
	, mTopPath(path)	
{

}

void Chown::accept(std::string path) 
{
    File file(path);
	
    if (file.isDirectory())
        file.listFiles(this);
    
    if (mFileChangedListener
		&& mPermissionManager 
		&& mPermissionManager->checkDelete(mOperUser, path))
    	mFileChangedListener->onOwnerChanged(mOperUser, mToUser, file.path());
}

void Chown::start() 
{	
    accept(mTopPath);
}

ChownOper::ChownOper(int code, std::string request) 
	: FileOper(code, request)
{
	
}	

Result ChownOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	std::string toUser = requestObj.getString("ToUser");
	
	Chown chwon(operUser, toUser, path);
	chwon.setPermissionManager(FileDBManager::getInstance());
	chwon.setFileChangedListener(FileDBManager::getInstance());
	chwon.start();	
	return Result::OK;
}	
	
} // namespace IBox



