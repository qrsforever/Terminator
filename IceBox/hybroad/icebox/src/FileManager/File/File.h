#ifndef __File__H_
#define __File__H_

#include "Type.h"
#include "FileFilter.h"
#include "DateTime.h"

#include <string>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace IBox {

class File {
public:
	File(std::string path);
	~File();
    void init();
	bool isFile() { return S_ISREG(mStat.st_mode); }
	bool isDirectory() { return S_ISDIR(mStat.st_mode); }
	std::string path() { return mPath; }
	std::string name() { return mName; }
	long size() { return mStat.st_size; }	
	int type();
	time_t createdTime() { return mStat.st_ctime; }	
	time_t lastModifiedTime() { return mStat.st_mtime; }	

	void listFiles(FileFilter* filter);	
	bool copyTo(std::string destPath);
	bool renameTo(std::string newPath);
	bool mkDir();
	bool mkDirs();
	bool mkParentDir();
	bool remove();	
	bool exists();
	std::string getParent();
	std::string hashCode();
	bool encrypt(std::string key);
	bool decrypt(std::string key);
	static bool lock(std::string path);
	static void unlock(std::string path);
	static bool isLocked(std::string path);

private:
	std::string mPath;
	std::string mName;
	struct stat mStat;

};

} // namespace IBox

#endif
