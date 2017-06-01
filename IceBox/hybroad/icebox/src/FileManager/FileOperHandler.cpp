#include "FileExplorerAssertions.h"
#include "FileOperHandler.h"
#include "JsonObject.h"
#include "ShareOper.h"
#include "MoveOper.h"
#include "CopyOper.h"
#include "CleanOper.h"
#include "ScanOper.h"
#include "MakeDirOper.h"
#include "ChownOper.h"
#include "RecoverOper.h"
#include "RemoveOper.h"
#include "EncryptOper.h"
#include "MakeDiffDBOper.h"
#include "ChangeLabelOper.h"
#include "MakeThumbnailOper.h"
#include "FileOperThreadManager.h"

#include <stdio.h>

namespace IBox {

FileOperHandler::FileOperHandler() 
{
   
}

FileOperHandler::~FileOperHandler() 
{
	
}

Result FileOperHandler::handleRequest(int code, std::string& request)
{
	logMsg("Code: 0x%x, Request: %s.\n", code, request.c_str());

	if (!isJsonString(request.c_str()))
		return Result::NoHandle;
	
	Result result = Result::OK;
	JsonObject jsonObj(request);
	
    std::string cmd = jsonObj.getString("Command");
	if (cmd.compare("Refresh") == 0) {
		MakeDiffDBOper* oper = new MakeDiffDBOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Share") == 0 || cmd.compare("Unshare") == 0) {
        ShareOper* oper = new ShareOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
    } else if (cmd.compare("Move") == 0) {
    	MoveOper* oper = new MoveOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Copy") == 0) {
		CopyOper* oper = new CopyOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("MakeDir") == 0) {
		MakeDirOper* oper = new MakeDirOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Delete") == 0) {
		RemoveOper* oper = new RemoveOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Undelete") == 0) {
		RecoverOper* oper = new RecoverOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Insert") == 0) {
		ScanOper* oper = new ScanOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("ChangeOwner") == 0) {
		ChownOper* oper = new ChownOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("GetThumbnail") == 0) {
		MakeThumbnailOper* oper = new MakeThumbnailOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("SetLabel") == 0) {
		ChangeLabelOper* oper = new ChangeLabelOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("Encrypt") == 0 || cmd.compare("Decrypt") == 0) {
		EncryptOper* oper = new EncryptOper(code, request);
		FileOperThreadManager::getInstance()->execute(oper);
	} else if (cmd.compare("CleanTrash") == 0) {
		if (jsonObj.getString("Path").empty()) {
			CleanTrashOper* oper = new CleanTrashOper(code, request);
			FileOperThreadManager::getInstance()->execute(oper);
	    } else {
	     	CleanOper* oper = new CleanOper(code, request);   
			FileOperThreadManager::getInstance()->execute(oper);
	    }
	} else {
		result = Result::NoHandle;
	}
	
	return result;
}

void FileOperHandler::response(int code, std::string request, Result result)
{
	
}
	
static FileOperHandler* gFileOperHandler = NULL;

FileOperHandler* FileOperHandler::getInstance() 
{
    if (!gFileOperHandler)
        gFileOperHandler = new FileOperHandler();
    return gFileOperHandler;
}

void FileOperHandler::releaseInstance() 
{
    if (gFileOperHandler) {
        delete gFileOperHandler;
        gFileOperHandler = NULL;
    }
}

} // namespace IBox


