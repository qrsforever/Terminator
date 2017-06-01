#include "FileExplorerAssertions.h"
#include "BackupOper.h"
#include "Thumbnail.h"
#include "Message.h"
#include "File.h"
#include "DiskManager.h"
#include "UserManager.h"
#include "FileDBManager.h"

#include <unistd.h>

namespace IBox {

Backup::Backup(std::string src, std::string dest) 
	: mSrcTopDir(src)
	, mDestTopDir(dest)	
{

}

void Backup::accept(std::string path) 
{
	File srcFile(path);

	if (DiskManager::getInstance()->localDiskDeviceCount() < 2 
		|| UserManager::getInstance()->getOnlineUserNumber() > 0) {
		logMsg("Not idle state or Not enough hard disk to backup files.\n");
		return ;
	}
	
	if (mFileNameFilter && mFileNameFilter->check(path)) 
		return ;
	
    std::string destPath = srcFile.path();
    destPath.replace(0, mSrcTopDir.size(), mDestTopDir);
    File destFile(destPath);

    if (srcFile.isFile()) {
		if (!FileDBManager::getInstance()->needBackup(srcFile.path())) 
			return ;
				
        if (destFile.exists()) {
            if (srcFile.hashCode().compare(destFile.hashCode()) == 0) {
				return ;
            }	
        }
        if (srcFile.copyTo(destPath)) {
            logMsg("Backup file {%s} successfully.\n", srcFile.path().c_str());
            if (mFileChangedListener)
                mFileChangedListener->onFileBackuped(srcFile.path(), destPath);
        }
    } else if (srcFile.isDirectory()) {
    	if (!destFile.exists() && destFile.mkDir() && mFileChangedListener)
            mFileChangedListener->onDirBackuped(srcFile.path(), destPath);
        srcFile.listFiles(this);
		if (mFileChangedListener)
            mFileChangedListener->onSizeChanged(destPath);
    }
}

void Backup::start() 
{	
    File srcTopDir(mSrcTopDir);
    srcTopDir.listFiles(this);
}	

Result BackupOper::backup()
{
	ExcludeFilesFilter filter;
	filter.add("*.thumbnail");
    filter.add("*.hyicon");
	filter.add("*/icebox/icebox.db");
    filter.add("*/icebox/tmp");
	filter.add("*/icebox/backup");
	filter.add("*.partitions");

	for (int i = 0; i < DiskManager::getInstance()->localDiskCount(); i++) {
		std::string src = DiskManager::getInstance()->getLocalDiskTopDir(i);
		std::string dest = DiskManager::getInstance()->getBackupDir(src);
		if (!src.empty() 
			&& !dest.empty()
			&& FileDBManager::getInstance()->diskIsReady(src)
			&& FileDBManager::getInstance()->diskIsReady(dest)) {
			Backup bak(src, dest);
			bak.setFileNameFilter(&filter);
			bak.setFileChangedListener(FileDBManager::getInstance());
			bak.start();	
		}
	}
	
}

} // namespace IBox

