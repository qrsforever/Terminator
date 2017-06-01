#ifndef __FileTable__H_
#define __FileTable__H_

#include "DBTable.h"
#include <list>
#include <utility>      // std::pair

namespace IBox {

typedef struct {
    int 		fileID;
    int			folderID;
    int			size;
    std::string hash;
    std::string	deviceName;
    std::string	owner;
    int 		type;
	std::string labels;
	std::string name;
    std::string ctime;
    std::string mtime;
	std::string artist;
	std::string album;
	std::string stime;  //shooting date
	std::string location;
	std::string sdevice; //shooting device
	int			stat;
	std::string shareWith;
	std::string chown;
	int			backup;
	int			encrypt;
}FileInfo;

class FileTable : public DBTable {
public:
	FileTable(SQLiteDatabase* db);
	~FileTable();
    
    bool insert(FileInfo file, int opid);
	bool insert(FileInfo file, int fileID, int opid);
    bool update(FileInfo file, int opid);
    bool update(int fileID, std::string key, int value, int opid);
    bool update(int fileID, std::string key, std::string value, int opid);
    bool remove(int fileID);
	bool remove(std::string fileName, int folderID, int opid, bool realRemoveFile);
	bool recover(std::string fileName, int folderID, int opid);
	size_t count(int folderID);
	int fileCount(int folderID);
	std::string getFileHashCode(std::string fileName, int folderID);
	int getFileID(std::string fileName, int folderID);
	FileInfo get(std::string fileName, int folderID);
	bool fileExists(std::string fileName, int folderID);  
	void listDeletedFiles(std::string user, std::vector<FileInfo>& files); 
	void listNeedBackupFiles(std::vector<FileInfo>& files); 
};

} // namespace IBox
#endif
