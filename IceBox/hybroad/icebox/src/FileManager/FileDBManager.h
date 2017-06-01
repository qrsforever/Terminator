#ifndef __FileExp__H_
#define __FileExp__H_

#include "FileChangedListener.h"
#include "PermissionManager.h"
#include "FileDatabase.h"
#include "VDisk.h"
#include "Handler.h"

namespace IBox {

class FileDBManager : public FileChangedListener, public PermissionManager {
public:
    ~FileDBManager();
	static FileDBManager* getInstance(); 
	static void releaseInstance();

	std::string diffDatabase(std::string username, int opid, std::string path);

	FileDatabase* addFileDatabase(std::string dir);
	void removeFileDatabase(std::string dir);
	FileDatabase* getFileDatabase(std::string path);
	int getDiskIndex(std::string dir);
	std::string getDiskAlias(std::string dir);
	void setDiskAlias(std::string dir, std::string alias);
	bool diskIsReady(std::string dir);
	void onDiskMounted(VDisk* vd);
	void onDiskUMounted(VDisk* vd);
	void onFileCreated(std::string username, std::string path);
    void onDirCreated(std::string username, std::string path);
    void onFileRenamed(std::string oldPath, std::string newPath);
    void onDirRenamed(std::string oldPath, std::string newPath);
    void onFileDeleted(std::string username, std::string path);
    void onDirDeleted(std::string username, std::string path);
	void onDirRealDeleted(std::string username, std::string path);
	void onFileRealDeleted(std::string username, std::string path);
	void onSizeChanged(std::string path);
	void onOwnerChanged(std::string username, std::string toUser, std::string path);
	void onFileRecovered(std::string username, std::string path); 
	void onDirRecovered(std::string username, std::string path); 
	void onFileBackuped(std::string src, std::string dest);
	void onDirBackuped(std::string src, std::string dest);
	void onShareChanged(std::string user, std::string toUser, std::string path, bool shared);
	void onHashCodeChanged(std::string path, std::string hash);
	void onFileEncrypted(std::string src, std::string dest);
	void onFileDecrypted(std::string src, std::string dest);
	bool needBackup(std::string path);
	bool checkAccess(std::string user, std::string path);
	bool checkDelete(std::string user, std::string path);
	void listDeletedFiles(std::string user, std::vector<std::string>& deletedFiles);
	void setLabel(std::string user, std::string label, std::string path);
	std::string getFileHashCode(std::string path);
private:	
	std::vector<FileDatabase*> 	mFileDBArray;
	FileDBManager();
};

} /* namespace IBox */

#endif // __FileExp__H_
