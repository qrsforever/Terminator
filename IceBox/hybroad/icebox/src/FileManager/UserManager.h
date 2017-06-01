#ifndef __UserManager__H_
#define __UserManager__H_

#include "SQLiteDatabase.h"
#include "UserTable.h"
#include "Result.h"
#include "Handler.h"

/*lihualong*/
#include "SysTime.h"
#include "LoginTable.h"

namespace IBox {

class UserManager : public Handler {
public:
	~UserManager();
	static UserManager* getInstance(); 
	static void releaseInstance();

	void addHistory(std::string jID, Result res);
		
	Result signIn(std::string jID, std::string username, std::string password);
	Result signOut(std::string jID);
	std::string getUserName(std::string jID);
	std::vector<UserInfo> listUsers();
	std::vector<std::string> getAllJID();
	//Result check(std::string user);
    std::vector<UserInfo>& getOnlineUsers() { return mOnlineUsers; }
	int getOnlineUserNumber() { return mOnlineUsers.size(); }
	std::string getUserSafeKey(std::string jID);
	Result setUserSafeKey(std::string jsonStr);
	Result getUserSafeKey(std::string jsonStr, std::string& safeKey);
	Result removeAccount(std::string jsonStr); 
	Result login(std::string jsonStr); 
	Result logout(std::string jsonStr); 
	Result addAccount(std::string jsonStr);

	void response(int code, std::string request, Result result);
	Result handleRequest(int code, std::string& request);
	
private:
	UserManager();
	SQLiteDatabase* 	mUsersDB;
	UserTable*			mUserTable;
	LoginTable*         mLoginTable;
	std::vector<UserInfo> 	mOnlineUsers;    
};

} // namespace IBox

#endif

