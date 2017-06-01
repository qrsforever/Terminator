#include "FileExplorerAssertions.h"
#include "File.h"
#include "LocalDisk.h"

namespace IBox {

BackupDir::BackupDir(std::string path)
	: mTopDir(path)
	, mPathDepth(0)
{
	
}

void BackupDir::accept(std::string path)
{
	mPathDepth++;
	logMsg("\t%s\t%d\n", path.c_str(), mPathDepth);
	File file(path);
	if (file.isDirectory()) {
		if (mPathDepth == 2)
			mDirs.push_back(file.path());
		if (mPathDepth < 3)
			file.listFiles(this);
	}	
	mPathDepth--;
}

std::vector<std::string> BackupDir::listDirs()
{
	accept(mTopDir);
	return mDirs;
}

CopyDir::CopyDir(std::string path)
	: mTopDir(path)
	, mPathDepth(0)
{
	
}

void CopyDir::accept(std::string path)
{
	mPathDepth++;
	logMsg("\t%s\t%d\n", path.c_str(), mPathDepth);
	File file(path);
	if (file.isDirectory()) {
		if (mPathDepth == 3)
			mDirs.push_back(file.path());
		if (mPathDepth < 4)
			file.listFiles(this);
	}	
	mPathDepth--;
}

std::vector<std::string> CopyDir::listDirs()
{
	accept(mTopDir);
	return mDirs;
}

LocalDisk::LocalDisk(std::string topDir)
	: VDisk(topDir)
{
	mBackupDir = getTopDir() + "/icebox/backup";
	mUserDir = getTopDir() + "/icebox/users";
	mOneKeyCopyDir = getTopDir() + "/icebox/devices";
	//mkDirs(mBackupDir);
	//mkDirs(mUserDir);
	//mkDirs(mOneKeyCopyDir);		
}

LocalDisk::~LocalDisk()
{
		
}

std::vector<std::string> LocalDisk::listBackupDirs()
{
	BackupDir bakDir(getBackupDir());
	return bakDir.listDirs();
}

std::vector<std::string> LocalDisk::listCopyDirs()
{
	BackupDir cpDir(getOneKeyCopyDir());
	return cpDir.listDirs();
}

} /* namespace IBox */