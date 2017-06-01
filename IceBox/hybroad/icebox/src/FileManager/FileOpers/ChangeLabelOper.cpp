#include "FileExplorerAssertions.h"
#include "MessageTypes.h"
#include "ChangeLabelOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"

#include <stdlib.h>
#include <vector>

namespace IBox
{

ChangeLabelOper::ChangeLabelOper(int code, std::string request) 
	: FileOper(code, request)
{
	
}	

Result ChangeLabelOper::execute() 
{
	JsonObject requestObj(mOperRequest);

	std::string path = requestObj.getString("Path");
	std::string label = requestObj.getString("Label");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	std::string cmd = requestObj.getString("Command");
	if (cmd.compare("SetLabel") == 0)
		FileDBManager::getInstance()->setLabel(operUser, label, path);
    
	return Result::OK;
}

} // namespace IBox





