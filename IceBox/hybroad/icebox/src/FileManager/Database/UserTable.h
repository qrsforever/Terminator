#ifndef __UserTable__H_
#define __UserTable__H_

#include "DBTable.h"
#include <list>

namespace IBox {

typedef struct {
    std::string name;
    std::string password;
    std::string phone;
	std::string alias;
	int			userID;
	std::string	jID;
	std::string safeKey;
} UserInfo;

class UserTable : public DBTable {
public:
	UserTable(SQLiteDatabase* db);
	~UserTable();
    
    bool insert(UserInfo info);
    bool update(std::string name, std::string key, std::string value);
    bool remove(std::string jID);
	UserInfo get(std::string name);
	std::vector<UserInfo> getUsers();
};

} // namespace IBox

#endif
