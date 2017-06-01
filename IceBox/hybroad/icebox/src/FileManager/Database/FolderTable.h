#ifndef __FolderTable__H_
#define __FolderTable__H_

#include "DBTable.h"
#include <list>

namespace IBox {

typedef struct {
    int			folderID;
    std::string name;
    std::string	deviceName;
	int			folder;
	std::string ctime;
    std::string mtime;
    int			stat;
	size_t 		size;
	std::string owner;
	std::string chown;
} FolderInfo;

class FolderTable : public DBTable {
public:
	FolderTable(SQLiteDatabase* db);
	~FolderTable();
    
    bool insert(FolderInfo folder, int opid);
	bool insert(FolderInfo folder, int folderID, int opid);
    bool update(FolderInfo folder, int opid);
    bool update(int folderID, std::string key, int value, int opid);
    bool update(int folderID, std::string key, std::string value, int opid);
    bool remove(int folderID);
	bool remove(std::string folderName, int folderID, int opid, bool realRemoveFile);
	bool recover(std::string folderName, int folderID, int opid);
	size_t count(int folderID);
	int dirCount(int folderID);
	int getFolderID(std::string name, int parentFolderID);
	FolderInfo get(std::string folderName, int parentFolderID);
	bool folderExists(std::string name, int parentFolderID); 
	std::string getNameByFolderID(int folderID);
	FolderInfo get(int folderID);
	void listDeletedFolders(std::string user, std::vector<FolderInfo>& folders); 
};

} // namespace IBox
#endif
