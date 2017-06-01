#ifndef __FileDatabase__H_
#define __FileDatabase__H_

#include "SQLiteDatabase.h"
#include "FileTable.h"
#include "FolderTable.h"

namespace IBox {

class FileDatabase {
public:
    FileDatabase(std::string topDir);
    ~FileDatabase();

	void insertFile(std::string who, std::string path);
    void insertFolder(std::string who, std::string path);
    void renameFile(std::string oldPath, std::string newPath);
    void renameDir(std::string oldPath, std::string newPath);
    void removeFile(std::string path, bool realRemoveFile);
    void removeDir(std::string path, bool realRemoveFile);
	void updateSize(std::string path);
	void recoverDir(std::string path);
	void recoverFile(std::string path);
	int getFolderID(std::string path);
	int fileCount(std::string path, std::string owner, bool i);
	void changeOwner(std::string toUser, std::string path);
	std::string getOwner(std::string path);
	void setBackupFlag(std::string path);
	void share(std::string who, std::string to, std::string path);
	void unshare(std::string who, std::string to, std::string path);
	void setLabel(std::string who, std::string label, std::string path);
	void encryptFile(std::string src, std::string dest);
	void decryptFile(std::string src, std::string dest);
	
	bool insertFile(FileInfo info);
	bool insertFolder(FolderInfo info);
	bool insertFolder(FolderInfo info, int folderID);
	bool updateFile(FileInfo info);
	bool updateFolder(FolderInfo info); 
	bool updateFile(int id, std::string key, std::string value);
	bool updateFolder(int id, std::string key, std::string value);
	bool updateFile(int id, std::string key, int value);
	bool updateFolder(int id, std::string key, int value);
	bool removeFile(std::string fileName, int folderID, bool realRemoveFile);
	bool removeFolder(std::string folderName, int folderID, bool realRemoveFile);
	bool recoverFile(std::string fileName, int folderID);
	bool recoverFolder(std::string fileName, int folderID);
	bool fileExists(std::string fileName, int folderID);
	bool folderExists(std::string folderName, int folderID);
	size_t getFolderSize(int folderID);
	std::string getFolderName(int folderID);
	int getFolderID(std::string folderName, int parentFolderID);
	FileInfo getFileInfo(std::string fileName, int folderID);
	FolderInfo getFolderInfo(std::string folderName, int folderID);
	std::string diff(std::string user, int opID);
	int getFileCount(int folderID, std::string owner, bool i);
	std::string getAbsolutePath(std::string name, int parentFolderID); 
	void listDeletedFiles(std::string owner, std::vector<std::string>& deletedFiles);
	void listNeedBackupFiles(std::list<std::string>& list); 
	bool isReady();
	void setReady();
	void setAlias(std::string alias);
	std::string getAlias();
	int getIndex(); 
	std::string getTopDir() { return mTopDir; }
	std::string getDBFilePath() { return mDBFilePath; }
	int getBackupFlag(std::string path);
	std::string getFileHashCode(std::string path);
	void setFileHashCode(std::string path, std::string hash);
private:
	std::string		mDBFilePath;
    SQLiteDatabase* mSQLiteDB;
    FileTable*      mFileTable;
    FolderTable*    mFolderTable;
	pthread_mutex_t mMutex;
	std::string     mTopDir;
	size_t nextOpid();
};

} // namespace IBox

#endif


