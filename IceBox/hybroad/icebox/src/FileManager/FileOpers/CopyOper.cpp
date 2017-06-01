#include "FileExplorerAssertions.h"
#include "File.h"
#include "ScanOper.h"
#include "MessageTypes.h"
#include "CopyOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "DiskManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"
#include "MakeDirOper.h"

#include <vector>

namespace IBox
{

Copy::Copy(std::string user, std::string src, std::string dest) 
	: mOperUser(user)
	, mSrcTopDir(src)
	, mDestTopDir(dest)	
	, mCopiedSize(0)
	, mCopiedNumber(0)
	, mTotalSize(0)
	, mTotalNumber(0)
{

}

void Copy::accept(std::string path) 
{
	File srcFile(path);
	
	if (mFileNameFilter && mFileNameFilter->check(path)) 
		return ;
	
    std::string destPath = srcFile.path();
    destPath.replace(0, mSrcTopDir.size(), mDestTopDir);
    File destFile(destPath);
    if (srcFile.isFile()) {
        if (destFile.exists()) {
            if (srcFile.hashCode().compare(destFile.hashCode()) == 0) {
				mCopiedNumber ++;
				mCopiedSize += srcFile.size();
				onProgressUpdate(mCopiedNumber, mCopiedSize);
				return ;
            }	

			std::string newPath;
			size_t pos = destFile.path().find_last_of(".");
			
            if (pos == std::string::npos)
				newPath = destFile.path() + "_" + DateTime::strftime(destFile.lastModifiedTime(), "YYYYMMDDHHMMSS");
			else 
				newPath = destFile.path().substr(0, pos) + "_"	
						  + DateTime::strftime(destFile.lastModifiedTime(), "YYYYMMDDHHMMSS")
						  + destFile.path().substr(pos);

			if (mPermissionManager 
				&& !mPermissionManager->checkDelete(mOperUser, destPath))
				return ;
			if (destFile.renameTo(newPath)
				&& mFileChangedListener) {
	    		mFileChangedListener->onFileRenamed(destPath, destFile.path());
			}
        }
		if (mPermissionManager 
			&& !mPermissionManager->checkAccess(mOperUser, srcFile.path()))
			return ;
		
        if (srcFile.copyTo(destPath)) {
			mCopiedNumber ++;
			mCopiedSize += srcFile.size();
            logMsg("Copy file {%s} successfully.\n", srcFile.path().c_str());
            if (mFileChangedListener)
                mFileChangedListener->onFileCreated(mOperUser, destPath);
        }

		if (mFileChangedListener != NULL) 
			mFileChangedListener->onSizeChanged(destFile.getParent());

		onProgressUpdate(mCopiedNumber, mCopiedSize);
    } else if (srcFile.isDirectory()) {
    	if (!destFile.exists() && destFile.mkDir() && mFileChangedListener)
            mFileChangedListener->onDirCreated(mOperUser, destPath);
        srcFile.listFiles(this);
		if (mFileChangedListener)
            mFileChangedListener->onSizeChanged(destPath);
    }
}

void Copy::start() 
{
	Scan scan(mOperUser, mSrcTopDir);
	scan.setFileNameFilter(mFileNameFilter);
	scan.setPermissionManager(mPermissionManager);
	scan.start();
	mTotalSize = scan.totalFileSize();
	mTotalNumber = scan.fileNumber();
	
    File srcTopDir(mSrcTopDir);
    srcTopDir.listFiles(this);
}

void Copy::onProgressUpdate(int copiedNum, size_t copiedSize)
{
	logMsg("Copied File Number: %d, Total File Numner: %d\n", copiedNum, mTotalNumber);
	logMsg("Copied File Size: %d, Total File Size: %d\n", copiedSize, mTotalSize);
	if (mFileOperProgressListener)
		mFileOperProgressListener->onProgressUpdate(mTotalNumber, mTotalSize, copiedNum, copiedSize);
}
	
CopyOper::CopyOper(int code, std::string request) 
	: FileOper(code, request)
{
	
}	

Result CopyOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string srcPath = requestObj.getString("Source");
	std::string destPath = requestObj.getString("Destination");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	
	ExcludeFilesFilter filter;
	filter.add("*/icebox/icebox.db");
	filter.add("*/icebox/tmp");
	filter.add("*.partitions");
	
    Copy cp(operUser, srcPath, destPath);
	cp.setFileNameFilter(&filter);
	cp.setPermissionManager(FileDBManager::getInstance());
	cp.setFileChangedListener(FileDBManager::getInstance());
	cp.setFileOperProgressListener(this);
	cp.start();	
	return Result::OK;
}

void CopyOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	int progress = copiedSize / (totalSize / 100);
	logMsg("Copy Progress: %d.\n", progress);
}	

CopyDiskOper::CopyDiskOper(std::string diskName, std::string destPath) 
	: FileOper(0, "") 
	, mTotalSize(0)
	, mCopiedSize(0)
	, mDiskName(diskName)
	, mDestPath(destPath)
{
	
}	

Result CopyDiskOper::execute() 
{
	std::vector< std::pair<std::string, std::string> > copyList; 

	ExcludeFilesFilter filter;
	filter.add("*.partitions");
	filter.add("*/icebox/tmp");
	filter.add("*/icebox/icebox.db");
	
	int partitionNum = DiskManager::getInstance()->getPartitionNumber(mDiskName);
	for (int i = 0; i < partitionNum; i++) {
        std::string src = DiskManager::getInstance()->getDiskTopDir(mDiskName, i);
        char dest[128] = {0};
        sprintf(dest, "%s/%d", mDestPath.c_str(), i);
        copyList.push_back(std::make_pair(src, dest));

    	Scan scan("public", src);
		scan.setFileNameFilter(&filter);
		scan.start();
		mTotalSize += scan.totalFileSize();
    }
	
    std::vector< std::pair<std::string, std::string> >::iterator it;
    for (it = copyList.begin(); it < copyList.end(); it++) {
		MakeDir mkdir("public", (*it).second);
		mkdir.setFileChangedListener(FileDBManager::getInstance());
		mkdir.start();
		
		Copy cp("public", (*it).first, (*it).second);
	    cp.setFileNameFilter(&filter);
		cp.setFileChangedListener(FileDBManager::getInstance());
	    cp.setFileOperProgressListener(this);
	    cp.start();
    }	

	return Result::OK;				
}

void CopyDiskOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	int progress = (mCopiedSize + copiedSize) / (mTotalSize / 100);
	if (totalSize == copiedSize)
		mCopiedSize += copiedSize;
	logMsg("onProgressUpdate, Progress: %d.\n", progress);

	NativeHandler& H = defNativeHandler();
	H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_PROGRESS, progress));
}	

} // namespace IBox


