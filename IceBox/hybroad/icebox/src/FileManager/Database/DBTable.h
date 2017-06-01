#ifndef __DBTable__H_
#define __DBTable__H_

#include "SQLiteDatabase.h"
#include "SQLiteResultSet.h"
#include "SQLiteValue.h"

#include <string>
#include <iostream>

namespace IBox {

class DBTable {
public :
    DBTable(SQLiteDatabase* db, std::string name);
    ~DBTable() {}

    void init();
    std::string name() {
        return mName;
    }
	int max(std::string key);

protected:
    SQLiteDatabase* mDB;
	std::vector< std::pair<int, std::string> > mUpdateHistory;
	
private:
    std::string mName;
	
	int latestVersion();
    bool update();
	int version();
    bool setVersion(int version);
    bool checkForUpdate();
};


} // namespace IBox

#endif
