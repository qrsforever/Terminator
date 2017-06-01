#include "LabelTable.h"

#include <stdio.h>
#include <string.h>

namespace IBox {

static const char* CreateTableSQLStatement = "CREATE TABLE labels(	\
	labelID INTEGER UNIQUE NOT NULL, 			\
	name TEXT UNIQUE NOT NULL PRIMARY KEY,  	\
	owners TEXT, type INTEGER)";


LabelTable::LabelTable(SQLiteDatabase* db)
    : DBTable(db, "labels") {
	mUpdateHistory.push_back(std::make_pair(1, CreateTableSQLStatement));
}

LabelTable::~LabelTable() {

}

bool LabelTable::insert(Label label) {
	SQLiteValue values[4];
	values[0] = SQLInt(max("labelID")+ 1);
	values[1] = SQLText(label.name.c_str());
	values[2] = SQLText(label.owners.c_str());
	values[3] = SQLInt(label.type);
	
    bool res = mDB->exec("INSERT INTO labels(labelID, name, owners, type) VALUES(?,?,?,?)", 
		values, sizeof(values) / sizeof(values[0]));
	return res;
}

bool LabelTable::update(std::string name, std::string owners) {
	const char* SQLStatement = "UPDATE labels SET owners = ? WHERE name == ?";
    return mDB->exec(SQLStatement, SQLText(owners.c_str()), SQLText(name.c_str()));
}

bool LabelTable::remove(std::string name) {
    return mDB->exec("DELETE FROM labels WHERE name == ?", SQLText(name.c_str()));
}

bool LabelTable::labelExists(std::string name) {
    int count = 0;
    const char* SQLStatement = "SELECT count(*) FROM labels WHERE name == ?";

    SQLiteResultSet* rs = mDB->query(SQLStatement, SQLText(name.c_str()));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }

    return (1 == count);
}

Label LabelTable::get(std::string name) {
    Label label;
	const char* SQLStatement = "SELECT * FROM labels WHERE name == ?";
    SQLiteResultSet* rs = mDB->query(SQLStatement, SQLText(name.c_str()));
    if (rs) {
		if (rs->next() && rs->columnText(1) != NULL) {
			label.id = rs->columnInt(0);
	        label.name  = rs->columnText(1);
	        label.owners = rs->columnText(2);
			label.type = rs->columnInt(3);
		}
        rs->close();
    }
	
    return label;
}

std::vector<Label> LabelTable::getLabels(std::string owner) {
	std::vector<Label> labels;
	std::string match = "%" + owner + "%";
	const char* SQLStatement = "SELECT * FROM labels WHERE type == ? OR owners LIKE ?";
    SQLiteResultSet* rs = mDB->query(SQLStatement, SQLInt(SYSTEM_LABLE), SQLText(match.c_str()));
	if (rs) {
	    while (rs->next()) {
			if (rs->columnText(1) != NULL) {
				Label label;
				label.id = rs->columnInt(0);
		        label.name  = rs->columnText(1);
				label.owners = rs->columnText(2);
		        label.type = rs->columnInt(3);
				labels.push_back(label);
			}
	    }
		rs->close();
	}
    return labels;
}

} // namespace IBox



