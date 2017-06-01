#include "FileExplorerAssertions.h"
#include "File.h"
#include "MessageTypes.h"
#include "MoveOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{

Move::Move(std::string operUser, std::string srcPath, std::string destPath)
	: mOperUser(operUser)
	, mSrcTopDir(srcPath)
	, mDestTopDir(destPath)	
{
	std::size_t found = mSrcTopDir.find_last_not_of("/");
	if (found != std::string::npos)
		mSrcTopDir.erase(found + 1);

	found = mDestTopDir.find_last_not_of("/");
	if (found != std::string::npos)
		mDestTopDir.erase(found + 1);
}

void Move::accept(std::string path) 
{
    File srcFile(path);

    std::string destPath = srcFile.path();
    destPath.replace(0, mSrcTopDir.size(), mDestTopDir);
    File destFile(destPath);

    if (srcFile.isFile()) {
        if (!destFile.exists() && srcFile.renameTo(destPath)) {
            FILEEXPLORER_LOG("Move file {%s} successfully.\n", srcFile.path().c_str());
            if (mFileChangedListener)
                mFileChangedListener->onFileRenamed(path, destPath);
        }
    } else if (srcFile.isDirectory()) {
    	if (!destFile.exists() && destFile.mkDir() && mFileChangedListener)
            mFileChangedListener->onDirCreated(mOperUser, destPath);
        srcFile.listFiles(this);
		if (mFileChangedListener != NULL) {
            mFileChangedListener->onSizeChanged(destPath);
            mFileChangedListener->onDirRealDeleted(mOperUser, srcFile.path());
        }    
    }
}

void Move::start() 
{	
    accept(mSrcTopDir);
}

MoveOper::MoveOper(int code, std::string request) 
	: FileOper(code, request)
	, mResult(Result::OK)
{
	
}	

Result MoveOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string srcPath = requestObj.getString("Source");
	std::string destPath = requestObj.getString("Destination");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";
	
    Move mv(operUser, srcPath, destPath);
	//mv.setFileNameFilter(&filter);
	//mv.setPermissionManager(FileDBManager::getInstance());
	mv.setFileChangedListener(FileDBManager::getInstance());
	//mv.setFileOperProgressListener(this);
	mv.start();	
	return Result::OK;
}

void MoveOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	

} // namespace IBox


