#include "FileExplorerAssertions.h"
#include "CalculateHashCodeOper.h"
#include "Message.h"
#include "File.h"
#include "DiskManager.h"
#include "UserManager.h"
#include "FileDBManager.h"

#include <unistd.h>

namespace IBox {

CalculateHashCode::CalculateHashCode(std::string path) 
	: mTopDir(path)
{

}

void CalculateHashCode::accept(std::string path) 
{
	File srcFile(path);

	if (UserManager::getInstance()->getOnlineUserNumber() > 0) {
		logMsg("Not idle state.\n");
		return ;
	}
	
	if (mFileNameFilter && mFileNameFilter->check(path)) 
		return ;

    if (srcFile.isFile()) {
		std::string hashCode = srcFile.hashCode();
		if (hashCode.compare(FileDBManager::getInstance()->getFileHashCode(path)) == 0) 
			return ;
				
        if (mFileChangedListener)
            mFileChangedListener->onHashCodeChanged(srcFile.path(), hashCode);
    } else if (srcFile.isDirectory()) { 
        srcFile.listFiles(this);
    }
}

void CalculateHashCode::start() 
{	
    File topDir(mTopDir);
    topDir.listFiles(this);
}	

Result CalculateHashCodeOper::calculate()
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
		if (!src.empty() 
			&& FileDBManager::getInstance()->diskIsReady(src)) {
			CalculateHashCode cal(src);
			cal.setFileNameFilter(&filter);
			cal.setFileChangedListener(FileDBManager::getInstance());
			cal.start();	
		}
	}
	
}

} // namespace IBox


