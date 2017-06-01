#ifndef __LocalDisk__H_
#define __LocalDisk__H_

#include "VDisk.h"
#include "FileFilter.h"

#include <vector>

namespace IBox {

class BackupDir : public FileFilter {
public:
    BackupDir(std::string path);
    std::vector<std::string> listDirs();
    void accept(std::string path);
private:
	std::string mTopDir;
	int mPathDepth;
	std::vector<std::string> mDirs;
};

class CopyDir : public FileFilter {
public:
    CopyDir(std::string path);
    std::vector<std::string> listDirs();
    void accept(std::string path);
private:
	std::string mTopDir;
	int mPathDepth;
	std::vector<std::string> mDirs;
};

class LocalDisk : public VDisk {
public:
    LocalDisk(std::string topDir);
	~LocalDisk();
	bool isLocalDisk() { return true; }
	std::string diskName() { return "Local Disk"; }
	std::string getBackupDir() { return mBackupDir; }
	std::string getUserDir() { return mUserDir; }
	std::string getOneKeyCopyDir() { return mOneKeyCopyDir; }	
	std::vector<std::string> listBackupDirs();
	std::vector<std::string> listCopyDirs();
	
private:
	std::string mBackupDir;
	std::string mUserDir;
	std::string mOneKeyCopyDir;
};

} /* namespace IBox */

#endif // __LocalDisk__H_