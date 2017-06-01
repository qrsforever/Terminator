#include "FileExplorerAssertions.h"

#include "UserTable.h"

namespace IBox {

static const std::string TABLE_NAME = "users";
static const std::string USER_NAME 	= "name";
static const std::string PASSWORD 	= "password";
static const std::string PHONE 		= "phone";
static const std::string ALIAS 		= "alias";
static const std::string USER_ID 	= "userID";
static const std::string JID 		= "jID";
static const std::string SAFEKEY 	= "safeKey";

UserTable::UserTable(SQLiteDatabase* db)
    : DBTable(db, TABLE_NAME) 
{
	int verno = 1;
	
    std::string CreateTable;
	CreateTable = "CREATE TABLE " + TABLE_NAME + "(";
	CreateTable.append(USER_NAME + " TEXT UNIQUE NOT NULL PRIMARY KEY,");
	CreateTable.append(PASSWORD + " TEXT,");
	CreateTable.append(PHONE + " TEXT,");
	CreateTable.append(ALIAS + " TEXT,");
	CreateTable.append(USER_ID + " INTEGER,");
	CreateTable.append(JID + " TEXT)");
	mUpdateHistory.push_back(std::make_pair(verno, CreateTable));

	verno++;
	std::string AlterTableV1 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV1.append("ADD " + SAFEKEY + " TEXT NULL;");
	mUpdateHistory.push_back(std::make_pair(verno, AlterTableV1));
}

UserTable::~UserTable() 
{
}

bool UserTable::insert(UserInfo info) 
{
    SQLiteValue values[7];
    values[0] = SQLText(info.name.c_str());
    values[1] = SQLText(info.password.c_str());
    values[2] = SQLText(info.phone.c_str());
    values[3] = SQLText(info.alias.c_str());
    values[4] = SQLInt(max(USER_ID) + 1);
	values[5] = SQLText(info.jID.c_str());
	values[6] = SQLText(info.safeKey.c_str());
	
    const char* SQLStatement = "INSERT INTO users(name, password, phone, alias, userID, jID, safeKey) VALUES(?,?,?,?,?,?,?)";
    return mDB->exec(SQLStatement, values, sizeof(values) / sizeof(values[0]));
}

bool UserTable::update(std::string name, std::string key, std::string value) 
{
	SQLiteValue values[2];
    values[0] = SQLText(value);
    values[1] = SQLText(name);

	std::string SQLStatement;
	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=? ");
	SQLStatement.append("WHERE " + USER_NAME + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool UserTable::remove(std::string jID) 
{
    return mDB->exec("DELETE FROM users WHERE name == ?", SQLText(jID.c_str()));
}

UserInfo UserTable::get(std::string name) {
    UserInfo userInfo;

    SQLiteResultSet* rs = mDB->query("SELECT * FROM users WHERE name = ?",
                                     SQLText(name.c_str()));
    if (rs) {
		if (rs->next()) {
	        userInfo.name  = rs->columnText(0);
	        userInfo.password  = rs->columnText(1);
	        userInfo.phone  = rs->columnText(2);
	        userInfo.alias    = rs->columnText(3);
	        userInfo.userID = rs->columnInt(4);
			userInfo.jID = rs->columnText(5);
			userInfo.safeKey = rs->columnText(6);
		}
        rs->close();
    }

    return userInfo;
}

std::vector<UserInfo> UserTable::getUsers() {
	std::vector<UserInfo> users;
    SQLiteResultSet* rs = mDB->query("SELECT * FROM users");
	if (rs) {
	    while (rs->next()) {
			if (rs->columnText(0) != NULL) {
				UserInfo user;
				user.name  = rs->columnText(0);
		        user.password  = rs->columnText(1);
		        user.phone  = rs->columnText(2);
		        user.alias    = rs->columnText(3);
		        user.userID = rs->columnInt(4);
				user.jID = rs->columnText(5);
				user.safeKey = rs->columnText(6);
				users.push_back(user);
			}
	    }
		rs->close();
	}
    return users;
}


} // namespace IBox


