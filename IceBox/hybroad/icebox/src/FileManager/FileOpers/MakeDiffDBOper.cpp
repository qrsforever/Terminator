#include "FileExplorerAssertions.h"
#include "MessageTypes.h"
#include "MakeDiffDBOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"

#include <stdlib.h>
#include <vector>

namespace IBox
{

MakeDiffDBOper::MakeDiffDBOper(int code, std::string request) 
	: FileOper(code, request)
{
	
}	

Result MakeDiffDBOper::execute() 
{
	JsonObject requestObj(mOperRequest);

	std::string path = requestObj.getString("Path");
	int opid = atoi(requestObj.getString("OPID").c_str());
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	std::string DBPath = FileDBManager::getInstance()->diffDatabase(operUser, opid, path);
    
    if (!DBPath.empty()) {
        requestObj.add("Path", DBPath);
		mOperRequest = requestObj.toString();
	}	
	
	return DBPath.empty() ? Result::DB_MakeDiffFailed : Result::OK;
}

} // namespace IBox




