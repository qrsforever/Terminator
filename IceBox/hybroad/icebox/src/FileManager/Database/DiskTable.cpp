#include "DiskTable.h"
#include <stdio.h>

namespace IBox {

static const std::string TABLE_NAME 		= "disks";
static const std::string DISK_ID 			= "id";
static const std::string FSNAME 			= "fsname";
static const std::string MNTDIR 			= "mntdir";
static const std::string TYPE 				= "type";
static const std::string TOTAL 				= "total";
static const std::string FREE 				= "free";
static const std::string ALIAS 				= "alias";
static const std::string BUSTYPE 			= "bustype";
static const std::string BACKUP_ID 			= "backupid";

DiskTable::DiskTable(SQLiteDatabase* db)
    : DBTable(db, TABLE_NAME) 
{
	std::string CreateTable = "CREATE TABLE " + TABLE_NAME + "(";
	CreateTable.append(DISK_ID + " INTEGER UNIQUE NOT NULL PRIMARY KEY,");
	CreateTable.append(FSNAME + " TEXT,");
	CreateTable.append(MNTDIR + " TEXT,");
	CreateTable.append(TYPE + " TEXT,");
	CreateTable.append(TOTAL + " INTEGER,");
	CreateTable.append(FREE + " INTEGER)");

	std::string AlterTableV1 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV1.append("ADD " + ALIAS + " TEXT NULL;");
	std::string AlterTableV2 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV2.append("ADD " + BUSTYPE + " TEXT NULL;");
	std::string AlterTableV3 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV3.append("ADD " + BACKUP_ID + " INTEGER DEFAULT 0;");
	
	mUpdateHistory.push_back(std::make_pair(1, CreateTable));
	mUpdateHistory.push_back(std::make_pair(2, AlterTableV1));
	mUpdateHistory.push_back(std::make_pair(3, AlterTableV2));
	mUpdateHistory.push_back(std::make_pair(4, AlterTableV3));
}

DiskTable::~DiskTable() 
{
}

bool 
DiskTable::insert(DiskInfo disk) 
{
    SQLiteValue values[9];
	std::string SQLStatement;
	
    values[0] = SQLInt(max(DISK_ID) + 1);
    values[1] = SQLText(disk.fsname.c_str());
    values[2] = SQLText(disk.mntdir.c_str());
    values[3] = SQLText(disk.type.c_str());
    values[4] = SQLInt(disk.total);
    values[5] = SQLInt(disk.free);
	values[6] = SQLText(disk.alias.c_str());
	values[7] = SQLText(disk.bustype.c_str());
	values[8] = SQLInt(disk.backupId);
	
	SQLStatement = "INSERT INTO " + TABLE_NAME + "(";
	SQLStatement.append(DISK_ID + ",");
	SQLStatement.append(FSNAME + ",");
	SQLStatement.append(MNTDIR + ",");
	SQLStatement.append(TYPE + ",");
	SQLStatement.append(TOTAL + ",");
	SQLStatement.append(FREE + ",");
	SQLStatement.append(ALIAS + ",");
	SQLStatement.append(BUSTYPE + ",");
	SQLStatement.append(BACKUP_ID + ") ");
	SQLStatement.append("VALUES(?,?,?,?,?,?,?,?,?)");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
DiskTable::update(DiskInfo disk) 
{
    remove(disk.id);
    return insert(disk);
}

bool 
DiskTable::update(int id, std::string key, int value) 
{
    SQLiteValue values[2];
	std::string SQLStatement;
	
    values[0] = SQLInt(value);
    values[1] = SQLInt(id);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=? ");
	SQLStatement.append("WHERE " + DISK_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
DiskTable::update(int id, std::string key, std::string value) 
{
    SQLiteValue values[2];
	std::string SQLStatement;
	
    values[0] = SQLText(value.c_str());
    values[1] = SQLInt(id);
	
    SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=? ");
	SQLStatement.append("WHERE " + DISK_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
DiskTable::remove(int id) 
{
	std::string SQLStatement;
	SQLStatement = "DELETE FROM " + TABLE_NAME + " WHERE " + DISK_ID + " == ?";
    return mDB->exec(SQLStatement.c_str(), SQLInt(id));
}

bool 
DiskTable::diskExists(std::string fsname) 
{
	std::string SQLStatement = "SELECT count(*) ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + FSNAME + " == ? ");
	
    int count = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(fsname.c_str()));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }
    return (1 == count);
}

DiskInfo 
DiskTable::get(int id) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + DISK_ID + " == ? ");
	
    DiskInfo diskInfo;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLInt(id));
    if (rs) {
		if (rs->next()) {
	        diskInfo.id  	 = rs->columnInt(0);
	        diskInfo.fsname  = rs->columnText(1);
	        diskInfo.mntdir  = rs->columnText(2);
	        diskInfo.type    = rs->columnText(3);
	        diskInfo.total   = rs->columnInt(4);
	        diskInfo.free 	 = rs->columnInt(5);
			diskInfo.alias   = rs->columnText(6);
			diskInfo.bustype = rs->columnText(7);
			diskInfo.backupId = rs->columnInt(8);
		}
        rs->close();
    }

    return diskInfo;
}

int 
DiskTable::nextId()
{
	return max(DISK_ID) + 1;
}

} // namespace IBox


