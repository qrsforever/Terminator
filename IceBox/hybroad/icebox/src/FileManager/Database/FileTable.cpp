#include "FileExplorerAssertions.h"
#include "FileTable.h"
#include "DateTime.h"

#include <stdio.h>

namespace IBox {

static const std::string TABLE_NAME 		= "files";
static const std::string FILE_ID 			= "fileID";
static const std::string FOLDER_ID 			= "folderID";
static const std::string SIZE 				= "size";
static const std::string HASH 				= "hash";
static const std::string DEVICENAME 		= "deviceName";
static const std::string OWNER 				= "owner";
static const std::string TYPE 				= "type";
static const std::string LABELS 			= "labels";
static const std::string NAME 				= "name";
static const std::string CREATED_DATE 		= "ctime";
static const std::string MODIFIED_DATE 		= "mtime";
static const std::string ARTIST 			= "artist";
static const std::string ALBUM 				= "album";
static const std::string SHOOTING_DATE 		= "stime";
static const std::string LOCATION 			= "location";
static const std::string SHOOTING_DEVICE 	= "shootDevice";
static const std::string STAT 				= "stat";
static const std::string SHARE_WITH 		= "shareWith";
static const std::string OPID 				= "opid";
static const std::string CHOWN 				= "chown";
static const std::string BACKUP 			= "backup";
static const std::string ENCRYPT 			= "encrypt";


FileTable::FileTable(SQLiteDatabase* db)
    : DBTable(db, TABLE_NAME) 
{
    std::string CreateTable;
	CreateTable = "CREATE TABLE " + TABLE_NAME + "(";
	CreateTable.append(FILE_ID + " INTEGER UNIQUE NOT NULL PRIMARY KEY,");
	CreateTable.append(FOLDER_ID + " INTEGER,");
	CreateTable.append(SIZE + " INTEGER,");
	CreateTable.append(HASH + " TEXT,");
	CreateTable.append(DEVICENAME + " TEXT,");
	CreateTable.append(OWNER + " TEXT,");
	CreateTable.append(TYPE + " INTEGER,");
	CreateTable.append(LABELS + " TEXT,");
	CreateTable.append(NAME + " TEXT,");
	CreateTable.append(CREATED_DATE + " TEXT,");
	CreateTable.append(MODIFIED_DATE + " TEXT,");
	CreateTable.append(ARTIST + " TEXT,");
	CreateTable.append(ALBUM + " TEXT,");
	CreateTable.append(SHOOTING_DATE + " TEXT,");
	CreateTable.append(LOCATION + " TEXT,");
	CreateTable.append(SHOOTING_DEVICE + " TEXT,");
	CreateTable.append(STAT + " INTEGER,");
	CreateTable.append(SHARE_WITH + " TEXT,");
	CreateTable.append(OPID + " INTEGER UNIQUE NOT NULL)");

	std::string AlterTableV1 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV1.append("ADD " + CHOWN + " TEXT NULL;");

	std::string AlterTableV2 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV2.append("ADD " + BACKUP + " INTEGER DEFAULT 1;");

	std::string AlterTableV3 = "ALTER TABLE " + TABLE_NAME + " ";
	AlterTableV3.append("ADD " + ENCRYPT + " INTEGER DEFAULT 0;");
	
	mUpdateHistory.push_back(std::make_pair(1, CreateTable));
	mUpdateHistory.push_back(std::make_pair(2, AlterTableV1));
	mUpdateHistory.push_back(std::make_pair(3, AlterTableV2));
	mUpdateHistory.push_back(std::make_pair(4, AlterTableV3));
}

FileTable::~FileTable() 
{
}

bool 
FileTable::insert(FileInfo file, int fileID, int opid) 
{
    SQLiteValue values[22];
	std::string SQLStatement;
	
    values[0] = SQLInt(fileID);
    values[1] = SQLInt(file.folderID);
    values[2] = SQLInt(file.size);
    values[3] = SQLText(file.hash);
    values[4] = SQLText(file.deviceName);
    values[5] = SQLText(file.owner);
    values[6] = SQLInt(file.type);
	values[7] = SQLText(file.labels);
    values[8] = SQLText(file.name);
    values[9] = SQLText(file.ctime);
    values[10] = SQLText(file.mtime);
	values[11] = SQLText(file.artist);
	values[12] = SQLText(file.album);
	values[13] = SQLText(file.stime);
	values[14] = SQLText(file.location);
	values[15] = SQLText(file.sdevice);
    values[16] = SQLInt(file.stat);
	values[17] = SQLText(file.shareWith);
    values[18] = SQLInt(opid);
	values[19] = file.chown.empty() ?  SQLText(file.owner) : SQLText(file.chown);
	values[20] = SQLInt(file.backup);
	values[21] = SQLInt(file.encrypt);
	
    SQLStatement = "INSERT INTO " + TABLE_NAME + "(";
	SQLStatement.append(FILE_ID + ",");
	SQLStatement.append(FOLDER_ID + ",");
	SQLStatement.append(SIZE + ",");
	SQLStatement.append(HASH + ",");
	SQLStatement.append(DEVICENAME + ",");
	SQLStatement.append(OWNER + ",");
	SQLStatement.append(TYPE + ",");
	SQLStatement.append(LABELS + ",");
	SQLStatement.append(NAME + ",");
	SQLStatement.append(CREATED_DATE + ",");
	SQLStatement.append(MODIFIED_DATE + ",");
	SQLStatement.append(ARTIST + ",");
	SQLStatement.append(ALBUM + ",");
	SQLStatement.append(SHOOTING_DATE + ",");
	SQLStatement.append(LOCATION + ",");
	SQLStatement.append(SHOOTING_DEVICE + ",");
	SQLStatement.append(STAT + ",");
	SQLStatement.append(SHARE_WITH + ",");
	SQLStatement.append(OPID + ",");
	SQLStatement.append(CHOWN + ",");
	SQLStatement.append(BACKUP + ",");
	SQLStatement.append(ENCRYPT + ") ");
	SQLStatement.append("VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::insert(FileInfo file, int opid) 
{
    SQLiteValue values[22];
	std::string SQLStatement;
	
    values[0] = SQLInt(max(FILE_ID) + 1);
    values[1] = SQLInt(file.folderID);
    values[2] = SQLInt(file.size);
    values[3] = SQLText(file.hash);
    values[4] = SQLText(file.deviceName);
    values[5] = SQLText(file.owner);
    values[6] = SQLInt(file.type);
    values[7] = SQLText(file.labels);
    values[8] = SQLText(file.name);
    values[9] = SQLText(file.ctime);
    values[10] = SQLText(file.mtime);
	values[11] = SQLText(file.artist);
	values[12] = SQLText(file.album);
	values[13] = SQLText(file.stime);
	values[14] = SQLText(file.location);
	values[15] = SQLText(file.sdevice);
    values[16] = SQLInt(file.stat);
	values[17] = SQLText(file.shareWith);
    values[18] = SQLInt(opid);	
	values[19] = file.chown.empty() ?  SQLText(file.owner) : SQLText(file.chown);
	values[20] = SQLInt(file.backup);
	values[21] = SQLInt(file.encrypt);
	
	SQLStatement = "INSERT INTO " + TABLE_NAME + "(";
	SQLStatement.append(FILE_ID + ",");
	SQLStatement.append(FOLDER_ID + ",");
	SQLStatement.append(SIZE + ",");
	SQLStatement.append(HASH + ",");
	SQLStatement.append(DEVICENAME + ",");
	SQLStatement.append(OWNER + ",");
	SQLStatement.append(TYPE + ",");
	SQLStatement.append(LABELS + ",");
	SQLStatement.append(NAME + ",");
	SQLStatement.append(CREATED_DATE + ",");
	SQLStatement.append(MODIFIED_DATE + ",");
	SQLStatement.append(ARTIST + ",");
	SQLStatement.append(ALBUM + ",");
	SQLStatement.append(SHOOTING_DATE + ",");
	SQLStatement.append(LOCATION + ",");
	SQLStatement.append(SHOOTING_DEVICE + ",");
	SQLStatement.append(STAT + ",");
	SQLStatement.append(SHARE_WITH + ",");
	SQLStatement.append(OPID + ",");
	SQLStatement.append(CHOWN + ",");
	SQLStatement.append(BACKUP + ",");
	SQLStatement.append(ENCRYPT + ") ");
	SQLStatement.append("VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::update(FileInfo file, int opid) 
{
    remove(file.fileID);
    return insert(file, file.fileID, opid);
}

bool 
FileTable::update(int fileID, std::string key, int value, int opid) 
{
	SQLiteValue values[3];
	std::string SQLStatement;
	
    values[0] = SQLInt(value);
    values[1] = SQLInt(opid);
	values[2] = SQLInt(fileID);
	
	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=?,");
	SQLStatement.append(OPID + "=? ");
	SQLStatement.append("WHERE " + FILE_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::update(int fileID, std::string key, std::string value, int opid) 
{
	SQLiteValue values[3];
	std::string SQLStatement;
	
    values[0] = SQLText(value);
    values[1] = SQLInt(opid);
	values[2] = SQLInt(fileID);
	
	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(key + "=?,");
	SQLStatement.append(OPID + "=? ");
	SQLStatement.append("WHERE " + FILE_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::recover(std::string fileName, int folderID, int opid) 
{
    int stat = 0;
    std::string SQLStatement;

	SQLStatement = "SELECT " + STAT + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
	
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(fileName), SQLInt(folderID));
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
	values[2] = SQLText(DateTime::now());
    values[3] = SQLText(fileName);
    values[4] = SQLInt(folderID);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(STAT + "=?,");
	SQLStatement.append(OPID + "=?,");
	SQLStatement.append(MODIFIED_DATE + "=? ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::remove(std::string fileName, int folderID, int opid, bool realRemoveFile) 
{
    int stat = 0;
    std::string SQLStatement;

    SQLStatement = "SELECT " + STAT + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(fileName.c_str()), SQLInt(folderID));
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
	values[2] = SQLText(DateTime::now());
    values[3] = SQLText(fileName);
    values[4] = SQLInt(folderID);

	SQLStatement = "UPDATE " + TABLE_NAME;
	SQLStatement.append(" SET ");
	SQLStatement.append(STAT + "=?,");
	SQLStatement.append(OPID + "=?,");
	SQLStatement.append(MODIFIED_DATE + "=? ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
	
    return mDB->exec(SQLStatement.c_str(), values, sizeof(values) / sizeof(values[0]));
}

bool 
FileTable::remove(int fileID) 
{
    std::string SQLStatement;
	SQLStatement = "DELETE FROM " + TABLE_NAME + " WHERE " + FILE_ID + " == ?";
    return mDB->exec(SQLStatement.c_str(), SQLInt(fileID));
}

bool 
FileTable::fileExists(std::string fileName, int folderID) 
{
	std::string SQLStatement = "SELECT count(*) ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
	
	int count = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLText(fileName.c_str()), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }
    return (1 == count);
}

size_t 
FileTable::count(int folderID) 
{
	std::string SQLStatement = "SELECT " + SIZE + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ?");

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
FileTable::fileCount(int folderID) 
{
	std::string SQLStatement = "SELECT count(*) ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + FOLDER_ID + " == ? ");
	SQLStatement.append("AND " + STAT + " != 2");

	int count = 0;
	SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	count = rs->columnInt(0);
        rs->close();
    }
	FILEEXPLORER_LOG("find %d files.\n", count);
    return count;
}

std::string 
FileTable::getFileHashCode(std::string fileName, int folderID) 
{
	std::string SQLStatement = "SELECT " + HASH + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");

	std::string hash;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(),SQLText(fileName.c_str()), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	hash = rs->columnText(0);
        rs->close();
    }
    return hash;
}

int 
FileTable::getFileID(std::string fileName, int folderID) 
{
	std::string SQLStatement = "SELECT " + FILE_ID + " ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
	
	int id = 0;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(),SQLText(fileName.c_str()), SQLInt(folderID));
    if (rs) {
		if (rs->next())
        	id = rs->columnInt(0);
        rs->close();
    }
    return id;
}

FileInfo 
FileTable::get(std::string fileName, int folderID) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + NAME + " == ? ");
	SQLStatement.append("AND " + FOLDER_ID + " == ?");
    FileInfo fileInfo;
    SQLiteResultSet* rs = mDB->query(SQLStatement.c_str(),SQLText(fileName.c_str()), SQLInt(folderID));
    if (rs) {
		if (rs->next()) {
	        fileInfo.fileID    = rs->columnInt(0);
	        fileInfo.folderID  = rs->columnInt(1);
	        fileInfo.size      = rs->columnInt(2);
	        fileInfo.hash      = rs->columnText(3);
	        fileInfo.deviceName  = rs->columnText(4);
	        fileInfo.owner     = rs->columnText(5);
	        fileInfo.type      = rs->columnInt(6);
			fileInfo.labels      = rs->columnText(7);
	        fileInfo.name      = rs->columnText(8);
	        fileInfo.ctime     = rs->columnText(9);
	        fileInfo.mtime     = rs->columnText(10);
			fileInfo.artist	   = rs->columnText(11);
			fileInfo.album     = rs->columnText(12);
			fileInfo.stime     = rs->columnText(13);
			fileInfo.location  = rs->columnText(14);
			fileInfo.sdevice   = rs->columnText(15);
	        fileInfo.stat      = rs->columnInt(16);
			fileInfo.shareWith = rs->columnText(17);
			fileInfo.chown 	   = rs->columnText(19);
			fileInfo.backup      = rs->columnInt(20);
			fileInfo.encrypt      = rs->columnInt(21);
		}
        rs->close();
    }
    return fileInfo;
}

void 
FileTable::listNeedBackupFiles(std::vector<FileInfo>& files) 
{
	std::string SQLStatement = "SELECT * ";
	SQLStatement.append("FROM " + TABLE_NAME + " ");
	SQLStatement.append("WHERE " + BACKUP + " == 1");
	
	SQLiteResultSet* rs = mDB->query(SQLStatement.c_str());
	if (rs) {
		while (rs->next()) {
			FileInfo fileInfo;
			fileInfo.fileID    = rs->columnInt(0);
			fileInfo.folderID  = rs->columnInt(1);
			fileInfo.size	   = rs->columnInt(2);
			fileInfo.hash	   = rs->columnText(3);
			fileInfo.deviceName  = rs->columnText(4);
			fileInfo.owner	   = rs->columnText(5);
			fileInfo.type	   = rs->columnInt(6);
			fileInfo.labels    = rs->columnText(7);
			fileInfo.name	   = rs->columnText(8);
			fileInfo.ctime	   = rs->columnText(9);
			fileInfo.mtime	   = rs->columnText(10);
			fileInfo.artist    = rs->columnText(11);
			fileInfo.album	   = rs->columnText(12);
			fileInfo.stime	   = rs->columnText(13);
			fileInfo.location  = rs->columnText(14);
			fileInfo.sdevice   = rs->columnText(15);
			fileInfo.stat	   = rs->columnInt(16);
			fileInfo.shareWith = rs->columnText(17);
			fileInfo.chown	   = rs->columnText(19);
			fileInfo.backup      = rs->columnInt(20);
			fileInfo.encrypt      = rs->columnInt(21);
			files.push_back(fileInfo);
		}
		rs->close();
	}
}

void 
FileTable::listDeletedFiles(std::string user, std::vector<FileInfo>& files) 
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
			FileInfo fileInfo;
	        fileInfo.fileID    = rs->columnInt(0);
	        fileInfo.folderID  = rs->columnInt(1);
	        fileInfo.size      = rs->columnInt(2);
	        fileInfo.hash      = rs->columnText(3);
	        fileInfo.deviceName  = rs->columnText(4);
	        fileInfo.owner     = rs->columnText(5);
	        fileInfo.type      = rs->columnInt(6);
			fileInfo.labels    = rs->columnText(7);
	        fileInfo.name      = rs->columnText(8);
	        fileInfo.ctime     = rs->columnText(9);
	        fileInfo.mtime     = rs->columnText(10);
			fileInfo.artist	   = rs->columnText(11);
			fileInfo.album     = rs->columnText(12);
			fileInfo.stime     = rs->columnText(13);
			fileInfo.location  = rs->columnText(14);
			fileInfo.sdevice   = rs->columnText(15);
	        fileInfo.stat      = rs->columnInt(16);
			fileInfo.shareWith = rs->columnText(17);
			fileInfo.chown 	   = rs->columnText(19);
			fileInfo.backup      = rs->columnInt(20);
			fileInfo.encrypt      = rs->columnInt(21);
			files.push_back(fileInfo);
		}
        rs->close();
    }
}


} // namespace IBox

