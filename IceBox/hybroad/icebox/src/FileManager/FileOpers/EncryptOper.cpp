#include "FileExplorerAssertions.h"
#include "File.h"
#include "MessageTypes.h"
#include "EncryptOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{

EncryptOper::EncryptOper(int code, std::string request) 
	: FileOper(code, request)
{
	
}	

Result EncryptOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string path = requestObj.getString("Path");
	std::string cmd = requestObj.getString("Command");
	std::string key = UserManager::getInstance()->getUserSafeKey(requestObj.getString("JID"));
	File file(path);
	if (cmd.compare("Encrypt") == 0) {
		if (file.encrypt(key)) {
			FileDBManager::getInstance()->onFileEncrypted(path, file.path());
			return Result::OK;
		}
		return Result::File_EncryptFailed;
	} else if (cmd.compare("Decrypt") == 0) {
		if (file.decrypt(key)) {
			FileDBManager::getInstance()->onFileDecrypted(path, file.path());
			return Result::OK;
		}
		return Result::File_DecryptFailed;
	}
	
	return Result::Command_NotFound;
}	
	
} // namespace IBox



