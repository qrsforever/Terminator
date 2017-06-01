#include "FileExplorerAssertions.h"

#include "UserManager.h"
#include "JsonObject.h"
#include "JsonObjectArray.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "DiskManager.h"
#include "MakeDirOper.h"
#include "FileOperThreadManager.h"

#include "File.h"

#include <stdio.h>

namespace IBox {

UserManager::UserManager() 
{
    mUsersDB = new SQLiteDatabase();
    if (mUsersDB) {
		File db("/root/database/users.db");
		if (!db.exists()) 
			db.mkParentDir();

		mUsersDB->open(db.path());
		
        mUserTable = new UserTable(mUsersDB);
        if (mUserTable) 
			mUserTable->init();
		mLoginTable = new LoginTable(mUsersDB);
        if (mLoginTable) 
			mLoginTable->init();
    }
}

UserManager::~UserManager() 
{
    if (mUserTable != NULL) {
        delete mUserTable;
        mUserTable = NULL;
    }
	if (mLoginTable != NULL) {
        delete mLoginTable;
        mLoginTable = NULL;
    }
    if (mUsersDB != NULL) {
        delete mUsersDB;
        mUsersDB = NULL;
    }
}

void UserManager::addHistory(std::string jID, Result result)
{
	LoginInfo inf;
	inf.name = getUserName(jID);
	inf.datetime = DateTime::now();
	inf.jID = jID;
	inf.errcode = result;
	mLoginTable->insert(inf);
}

Result UserManager::signIn(std::string jID, std::string username, std::string password) 
{
	UserInfo info = mUserTable->get(username);
	if (info.name.empty()) {
		FILEEXPLORER_LOG_WARNING("Unregistered user.\n");
        return Result::User_Unregistered;
	}
	
	if (password.compare(info.password) != 0) {
        FILEEXPLORER_LOG_WARNING("Wrong password.\n");
		return Result::User_WrongPassword;
	}

	info.jID = jID;
	
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (jID.compare((*it).jID) == 0) { 
			return Result::OK;
        }	
    }
	mOnlineUsers.push_back(info);
    return Result::OK;
}

Result UserManager::signOut(std::string jID) 
{
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (jID.compare((*it).jID) == 0) {
			mOnlineUsers.erase(it);
			break;
		}	
    }
	return Result::OK;
}

std::vector<UserInfo> UserManager::listUsers() 
{
	return mUserTable->getUsers();
}

std::vector<std::string> UserManager::getAllJID() 
{
	std::vector<std::string> JIDs;
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
		JIDs.push_back((*it).jID);
    }
	return JIDs;
}	

std::string UserManager::getUserName(std::string jID) 
{
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (jID.compare((*it).jID) == 0) {
			return (*it).name;
		}	
    }
	return "";
}

std::string UserManager::getUserSafeKey(std::string jID)
{
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (jID.compare((*it).jID) == 0) {
			return (*it).safeKey;
		}	
    }
	return "";
}

Result UserManager::setUserSafeKey(std::string jsonStr)
{
	JsonObject jsonObj(jsonStr);
	std::string username = getUserName(jsonObj.getString("JID"));
	std::string safeKey = jsonObj.getString("SafeKey");
	
	UserInfo info = mUserTable->get(username);
	if (info.name.empty()) {
		FILEEXPLORER_LOG_WARNING("Unregistered user.\n");
        return Result::User_Unregistered;
	}
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (username.compare((*it).name) == 0) {
			break;
		}	
    }

	if (it == mOnlineUsers.end())
		return Result::User_NotOnline;

	if (mUserTable->update(username, "safeKey", safeKey)) {
		for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
	        if (username.compare((*it).name) == 0) {
				(*it).safeKey = safeKey;
			}	
	    }
		return Result::OK;
	}
	return Result::DB_UpdateFailed;
}

Result UserManager::getUserSafeKey(std::string jsonStr, std::string& safeKey)
{
	JsonObject jsonObj(jsonStr);
	std::string username = getUserName(jsonObj.getString("JID"));
	
	UserInfo info = mUserTable->get(username);
	if (info.name.empty()) {
		FILEEXPLORER_LOG_WARNING("Unregistered user.\n");
        return Result::User_Unregistered;
	}
	std::vector<UserInfo>::iterator it;
    for (it = mOnlineUsers.begin(); it != mOnlineUsers.end(); it++) {
        if (username.compare((*it).name) == 0) {
			safeKey = (*it).safeKey;
			return Result::OK;
		}	
    }

	return Result::User_NotOnline;
}

Result UserManager::addAccount(std::string jsonStr) 
{	
	Result result = Result::OK;
	JsonObject jsonObj(jsonStr);

	std::string username = jsonObj.getString("UserName");
	std::string password = jsonObj.getString("Password");
	std::string jID = jsonObj.getString("JID");

	UserInfo info = mUserTable->get(username);
	if (info.name.empty()) {
		info.name = username;
		info.password = password;
		info.jID = "";
		info.phone = "";
		info.alias = "";
		info.safeKey = "";
	    result = mUserTable->insert(info) ? Result::OK : Result::DB_InsertFailed;
	} 

	if (result == Result::OK) {
		MakeUserDirsOper* oper = new MakeUserDirsOper(username, 
			DiskManager::getInstance()->getUserDir(username));
		FileOperThreadManager::getInstance()->execute(oper);
	}
	
	return result;
}

Result UserManager::removeAccount(std::string jsonStr) 
{
	Result result = Result::OK;
	JsonObject jsonObj(jsonStr);

	std::string username = jsonObj.getString("UserName");
	std::string password = jsonObj.getString("Password");
	
	UserInfo info = mUserTable->get(username);
	if (!info.name.empty()) {
		if (password.compare(info.password) == 0) 
			result = mUserTable->remove(username) ? Result::OK : Result::DB_DeleteFailed;
		else {
			FILEEXPLORER_LOG_WARNING("Wrong password.\n");
			result = Result::User_WrongPassword;
		}	
	}
	return result;
}

Result UserManager::login(std::string jsonStr) 
{
	JsonObject jsonObj(jsonStr);

	std::string jID = jsonObj.getString("JID");
	std::string username = jsonObj.getString("UserName");
	std::string password = jsonObj.getString("Password");
	
	Result result = signIn(jID, username, password);
	
	addHistory(jID, result);

	return result;
}

Result UserManager::logout(std::string jsonStr) 
{
	JsonObject jsonObj(jsonStr);
	std::string jID = jsonObj.getString("JID");

	return signOut(jID);
}

Result UserManager::handleRequest(int code, std::string& request)
{
	logMsg("Code: 0x%x, Request: %s.\n", code, request.c_str());
	if (!isJsonString(request.c_str()))
		return Result::NoHandle;
	JsonObject jsonObj(request);
    std::string func = jsonObj.getString("Command");
	if (func.compare("RegisterBox") == 0) {
        return addAccount(request);
    } else if (func.compare("UnregisterBox") == 0) {
        return removeAccount(request);
    } else if (func.compare("LoginBox") == 0) {
        return login(request);
    } else if (func.compare("LogoutBox") == 0) {
        return logout(request);
    } else if (func.compare("ListUsers") == 0) {
    	JsonObjectArray jsonArray;
	    std::vector<UserInfo> users = UserManager::getInstance()->listUsers();
	    if (users.size() > 0) {
	        for (int i = 0; i < users.size(); i ++) {
	            jsonObj.clear();
	            jsonObj.add("name", users[i].name);
	            jsonArray.add(jsonObj.toString());
	        }
	    }
		JsonObject response(request);
	    response.add("Count", users.size());
	    if (users.size() > 0)
	        response.add("Users", jsonArray.toString());
        request = response.toString();
		return Result::OK;
    } else if (func.compare("SetSafeKey") == 0) {
        return setUserSafeKey(request);
    } else if (func.compare("GetSafeKey") == 0) {
    	std::string safeKey;
        Result res = getUserSafeKey(request, safeKey);
		if (res == Result::OK) {
			JsonObject response(request);
			response.add("SafeKey", safeKey);
			request = response.toString();
		}
		return res;
    } else if (getUserName(jsonObj.getString("JID")).empty()){
		return Result::User_NotOnline;
	}	 
	return Result::NoHandle;
}

void UserManager::response(int code, std::string request, Result result)
{
	JsonObject response(request);
	response.add("Action", "FunctionResponse");
	if (result == Result::OK) {
		response.add("Response", "Success");
	} else {
		response.add("Response", "Failed");
		response.add("ErrorCode", result);
	}
	
	StringData* strData = new StringData(response.toString().c_str());
	NativeHandler& H = defNativeHandler();
	H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_RESPONSE, code, strData));
	strData->safeUnref();
}
	
static UserManager* gUserManager = NULL;

UserManager* UserManager::getInstance() 
{
    if (!gUserManager)
        gUserManager = new UserManager();
    return gUserManager;
}

void UserManager::releaseInstance() 
{
    if (gUserManager) {
        delete gUserManager;
        gUserManager = NULL;
    }
}

} // namespace IBox


