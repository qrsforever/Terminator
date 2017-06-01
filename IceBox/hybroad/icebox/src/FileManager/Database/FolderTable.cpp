#include "FileExplorerAssertions.h"
#include "FolderTable.h"
#include "DateTime.h"

#include <stdio.h>

namespace IBox {

static const std::string TABLE_NAME 		= "folders";
static const std::string FOLDER_ID 			= "folderID";
static const std::string PARENT_FOLDER	 	= "folder";
static const std::string SIZE 				= "size";
static const std::string DEVICENAME 		= "deviceName";
static const std::string OWNER 				= "owner";
static const std::string NAME 				= "name";
static const std::string CREATED_DATE 		= "ctime";
static const std::string MODIFIED_DATE 		= "mtime";
static const std::string STAT 				= "stat";
static const std::string OPID 				= "opid";
static const std::string CHOWN 				= "chown";

FolderTable::FolderTable(SQLiteDatabase* db)
    : DBTable(db, TABLE_NAME) 
{
	std::string CreateTable;
	CreateTable = "CREATE TABLE " + TABLE_NAME + "(";
	CreateTable.append(FOLDER_ID + " INTEGER UNIQUE NOT NULL PRIMARY KEY,");
	CreateTable.append(NAME + " TEXT,");
	CreateTable.append(DEVICENAME + " TEXT,");
	CreateTable.append(PARENT_FOLDER + " INTEGER,");
	CreateTable.append(CREATED_DATE + " TEXT,");
	CreateTable.append(MODIFIED_DATE + " TEXT,");
	CreateTable.append(STAT + " INTEGER,");
	CreateTable.append(SIZE + " INTEGER,");
	CreateTable.append(OPID + " INTEGER UNIQUE NOT NULL)");

	std::string AlterTableV1 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV1.append("ADD " + OWNER + " TEXT NULL;");

	std::string AlterTableV2 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV2.append("ADD " + CHOWN + " TEXT NULL;");
	
    mUpdateHistory.push_back(std::make_pair(1, CreateTable));
	mUpdateHistory.push_back(std::make_pair(2, AlterTableV1));
	mUpdateHistory.push_back(std::make_pair(3, AlterTableV2));
}

FolderTable::~FolderTable() 
{
}

bool 
FolderTable::insert(FolderInfo folder, int opid) 
{
    SQLiteValue values[11];
	std::string SQLStatement;
	
    values[0] = SQLInt(max(FOLDER_ID) + 1);
    values[1] = SQLText(folder.name.c_str());
    values[2] = SQLText(folder.deviceName.c_str());
    values[3] = SQLInt(folder.folder);
    values[4] = SQLText(folder.ctime.c_str());
    values[5] = SQLText(folder.mtime.c_str());
    values[6] = SQLInt(folder.stat);
	values[7] = SQLInt(folder.size);
    values[8] = SQLInt(opid);
	values[9] = SQLText(folder.owner.c_str());
	values[10] = folder.chown.empty() ?  SQLText(folder.owner) : SQLText(folder.chown);
	
	SQLStatement = "INSERT INTO " + TABLE_NAME + "(";
	SQLStatement.append(FOLDER_ID + ",");
	SQLStatement.append(NAME + ",");
	SQLStatement.append(DEVICENAME + ",");
	SQLStatement.append(PARENT_FOLDER + ",");
	SQLStatement.append(CREATED_DATE + ",");
	SQLStatement.append(MODIFIED_DATE + ",");
	SQLStatement.append(STAT + ",");
	SQLStatement.append(SIZE + ",");
	SQLStatement.append(OPID + ",");
	SQLStatement.append(OWNER + ",");
	SQLStatement.append(CHOWN + ") ");
	SQLStatement.append("VALUES(?,?,?,?,?,?,?,?,?,?,?)");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FolderTable::insert(FolderInfo folder, int folderID, int opid) 
{
    SQLiteValue values[11];
	std::string SQLStatement;
	
    values[0] = SQLInt(folderID);
    values[1] = SQLText(folder.name.c_str());
    values[2] = SQLText(folder.deviceName.c_str());
    values[3] = SQLInt(folder.folder);
    values[4] = SQLText(folder.ctime.c_str());
    values[5] = SQLText(folder.mtime.c_str());
    values[6] = SQLInt(folder.stat);
    values[7] = SQLInt(folder.size);
    values[8] = SQLInt(opid);
	values[10] = folder.chown.empty() ?  SQLText(folder.owner) : SQLText(folder.chown);
	
	SQLStatement = "INSERT INTO " + TABLE_NAME + "(";
	SQLStatement.append(FOLDER_ID + ",");
	SQLStatement.append(NAME + ",");
	SQLStatement.append(DEVICENAME + ",");
	SQLStatement.append(PARENT_FOLDER + ",");
	SQLStatement.append(CREATED_DATE + ",");
	SQLStatement.append(MODIFIED_DATE + ",");
	SQLStatement.append(STAT + ",");
	SQLStatement.append(SIZE + ",");
	SQLStatement.append(OPID + ",");
	SQLStatement.append(OWNER + ",");
	SQLStatement.append(CHOWN + ") ");
	SQLStatement.append("VALUES(?,?,?,?,?,?,?,?,?,?,?)");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FolderTable::update(FolderInfo folder, int opid) 
{
    remove(folder.folderID);
    return insert(folder, folder.folderID, opid);
}

bool 
FolderTable::update(int id, std::string key, int value, int opid) 
{
    SQLiteValue values[3];
	std::string SQLStatement;
	
    values[0] = SQLInt(value);
    values[1] = SQLInt(opid);
	values[2] = SQLInt(id);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=?,");
	SQLStatement.append(OPID + "=? ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FolderTable::update(int id, std::string key, std::string value, int opid) 
{
    SQLiteValue values[3];
	std::string SQLStatement;
	
    values[0] = SQLText(value.c_str());
    values[1] = SQLInt(opid);
	values[2] = SQLInt(id);
	
    SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=?,");
	SQLStatement.append(OPID + "=? ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FolderTable::remove(int folderID) 
{
	std::string SQLStatement;
	SQLStatement = "DELETE FROM " + TABLE_NAME + " WHERE " + FOLDER_ID + " == ?";
    return mDB->exec(SQLStatement.c_str(), SQLInt(folderID));
}

bool 
FolderTable::remove(std::string folderName, int folderID, int opid, bool realRemoveFile) 
{
	std::string SQLStatement;

    SQLStatement = "SELECT " + STAT + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    int stat = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(folderName), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	stat = rs->columnInt(0);
        rs->close();
    }
	
    if (realRemoveFile) { 
		if (stat == 2)
			return true;
		stat = 2;
	} else {
	    if (stat != 0)
			return true;
		stat = 1;
	}
	
    SQLiteValue values[5];
    values[0] = SQLInt(stat);
    values[1] = SQLInt(opid);
	values[2] = SQLText(DateTime::now().c_str());
    values[3] = SQLText(folderName.c_str());
    values[4] = SQLInt(folderID);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(STAT + "=?,");
	SQLStatement.append(OPID + "=?,");
	SQLStatement.append(MODIFIED_DATE + "=? ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FolderTable::recover(std::string folderName, int folderID, int opid) 
{
	std::string SQLStatement;

    SQLStatement = "SELECT " + STAT + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    int stat = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(folderName), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	stat = rs->columnInt(0);
        rs->close();
    }
	
    if (stat == 0)
		return true;
	else if (stat == 1)
		stat = 0;
	else
		return false;
	
    SQLiteValue values[5];
    values[0] = SQLInt(stat);
    values[1] = SQLInt(opid);
	values[2] = SQLText(DateTime::now().c_str());
    values[3] = SQLText(folderName.c_str());
    values[4] = SQLInt(folderID);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(STAT + "=?,");
	SQLStatement.append(OPID + "=?,");
	SQLStatement.append(MODIFIED_DATE + "=? ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}


bool 
FolderTable::folderExists(std::string name, int parentFolderID) 
{
	std::string SQLStatement = "SELECT count(*) ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
	int count = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(name), SQLInt(parentFolderID));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }
    return (1 == count);
}

size_t 
FolderTable::count(int folderID) 
{
	std::string SQLStatement = "SELECT " + SIZE + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + PARENT_FOLDER + " == ?");
	
	size_t size = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLInt(folderID));
	if (rs) {
	    while (rs->next()) {
			size += rs->columnInt(0);
	    }
		rs->close();
	}
	return size;
}

int 
FolderTable::dirCount(int folderID) 
{
	std::string SQLStatement = "SELECT count(*) ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + PARENT_FOLDER + " == ? ");
	SQLStatement.append("AND " + STAT + " != 2");

	int count = 0;
	SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }

	FILEEXPLORER_LOG("find %d folders.\n", count);
    return count;
}

int 
FolderTable::getFolderID(std::string name, int parentFolderID) 
{
	std::string SQLStatement = "SELECT " + FOLDER_ID + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    int id = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(name), SQLInt(parentFolderID));
    if (rs) {
		if (rs->next())
        	id = rs->columnInt(0);
        rs->close();
    }
    return id;
}

std::string 
FolderTable::getNameByFolderID(int folderID) 
{
	std::string SQLStatement = "SELECT " + NAME + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ?");
	
    std::string name = "";
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(),SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	name = rs->columnText(0);
        rs->close();
    }
    return name;
}

FolderInfo 
FolderTable::get(std::string folderName, int parentFolderID) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + PARENT_FOLDER + " == ?");
	
    FolderInfo info;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(folderName), SQLInt(parentFolderID));
    if (rs) {
		if (rs->next()) {
	        info.folderID  = rs->columnInt(0);
	        info.name      = rs->columnText(1);
	        info.deviceName  = rs->columnText(2);
	        info.folder    = rs->columnInt(3);
	        info.ctime     = rs->columnText(4);
	        info.mtime     = rs->columnText(5);
	        info.stat     = rs->columnInt(6);
			info.size     = rs->columnInt(7);
			info.owner     = rs->columnText(9);
			info.chown		= rs->columnText(10) ? rs->columnText(10) : ""; 
    	}
        rs->close();
    }
    return info;
}

FolderInfo 
FolderTable::get(int folderID) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ?");
	
    FolderInfo info;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLInt(folderID));
    if (rs) {
		if (rs->next()) {
	        info.folderID  = rs->columnInt(0);
	        info.name      = rs->columnText(1);
	        info.deviceName  = rs->columnText(2);
	        info.folder    = rs->columnInt(3);
	        info.ctime     = rs->columnText(4);
	        info.mtime     = rs->columnText(5);
	        info.stat     = rs->columnInt(6);
			info.size     = rs->columnInt(7);
			info.owner     = rs->columnText(9);
			info.chown		= rs->columnText(10) ? rs->columnText(10) : ""; 
		}
        rs->close();
    }
    return info;
}

void 
FolderTable::listDeletedFolders(std::string user, std::vector<FolderInfo>& folders) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + STAT + " == 1 ");
	SQLStatement.append("AND (" + OWNER + " == ? ");
	SQLStatement.append("OR (" + OWNER + " == ? ");
	SQLStatement.append("AND (" + CHOWN + " == ? ");
	SQLStatement.append("OR " + CHOWN + " == ?)))");

	SQLiteValue values[4];
	values[0] = SQLText(user);
    values[1] = SQLText("public");
    values[2] = SQLText(user);
    values[3] = SQLText("public");
	
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
    if (rs) {
		while (rs->next()) {
			FolderInfo info;
	        info.folderID  = rs->columnInt(0);
	        info.name      = rs->columnText(1);
	        info.deviceName  = rs->columnText(2);
	        info.folder    = rs->columnInt(3);
	        info.ctime     = rs->columnText(4);
	        info.mtime     = rs->columnText(5);
	        info.stat     = rs->columnInt(6);
			info.size     = rs->columnInt(7);
			info.owner     = rs->columnText(9);
			info.chown		= rs->columnText(10) ? rs->columnText(10) : ""; 
			folders.push_back(info);
		}
        rs->close();
    }
}


} // namespace IBox

