#include "FileExplorerAssertions.h"
#include "DBTable.h"
#include <stdio.h>

namespace IBox {

DBTable::DBTable(SQLiteDatabase* db, std::string name) {
    mDB = db;
    mName = name;
}

void DBTable::init() {
    if (checkForUpdate())
        update();
}

int DBTable::version() {
    int ver = 0;
    if (mDB->tableExists("versions")) {
	    SQLiteResultSet* rs = mDB->query(
			"SELECT Version FROM versions WHERE TableName = ?", 
			SQLText(mName.c_str()));
	    if (rs) {
			if (rs->next())
	        	ver = rs->columnInt(0);
	        rs->close();
	    }
    }
	//FILEEXPLORER_LOG("The current version of table(%s): %d\n", mName.c_str(), ver);
    return ver;
}

bool DBTable::setVersion(int version) {
    if (!mDB->tableExists("versions")
        && !mDB->exec("CREATE TABLE versions(TableName TEXT UNIQUE NOT NULL PRIMARY KEY, Version INT)"))
        return false;

    int count = 0;
    SQLiteResultSet* rs = mDB->query(
		"SELECT count(*) FROM versions WHERE TableName == ?", 
		SQLText(mName.c_str()));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }
    if (count == 0)
        return mDB->exec("INSERT INTO versions(TableName, Version) VALUES(?,?)", 
        		SQLText(mName.c_str()), SQLInt(version));

    return mDB->exec("UPDATE versions SET Version=? WHERE TableName=?", 
			SQLInt(version), SQLText(mName.c_str()));
}

int DBTable::latestVersion() {
	std::vector< std::pair<int, std::string> >::iterator it;
	it = mUpdateHistory.end() - 1;
	//FILEEXPLORER_LOG("The latest version of table(%s): %d\n", mName.c_str(), (*it).first);
	return (*it).first;
}

bool DBTable::checkForUpdate() {
    return (latestVersion() > version());
}

bool DBTable::update() {
	std::vector< std::pair<int, std::string> >::iterator it;
	for (it = mUpdateHistory.begin(); it < mUpdateHistory.end(); it++) {
		if ((*it).first > version()) {
			FILEEXPLORER_LOG("Upgrading table(%s) from version %d to %d.\n",
                  		mName.c_str(),
                  		version(),
                  		(*it).first);
			if (!mDB->exec((*it).second.c_str()))
        		return false;
			setVersion((*it).first);
		}	
	}	
    return true;
}


int DBTable::max(std::string key) {
    int value = 0;
	const std::string SQLStatement = "SELECT max(" + key + ") FROM " + mName;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str());
    if (rs) {
		if (rs->next())
        	value = rs->columnInt(0);
        rs->close();
    }
    return value;
}

} // namespace IBox


