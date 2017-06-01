#include "FileExplorerAssertions.h"

#include "LoginTable.h"

namespace IBox {

static const char* CreateTableSQLStatement = "CREATE TABLE history(\
                                              id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT,\
                                              datetime TEXT, jid TEXT, errcode INTEGER DEFAULT 0)";

LoginTable::LoginTable(SQLiteDatabase* db)
    : DBTable(db, "history") {
	mUpdateHistory.push_back(std::make_pair(1, CreateTableSQLStatement));
}

LoginTable::~LoginTable() {

}

bool LoginTable::insert(LoginInfo info) 
{
    SQLiteValue values[4];
    values[0] = SQLText(info.name);
    values[1] = SQLText(info.datetime);
    values[2] = SQLText(info.jID);
    values[3] = SQLInt(info.errcode);

    const char* SQLStatement = "INSERT INTO history(name, datetime, jid, errcode) VALUES(?,?,?,?)";
    return mDB->exec(SQLStatement, values, sizeof(values) / sizeof(values[0]));
}

} // namespace IBox


