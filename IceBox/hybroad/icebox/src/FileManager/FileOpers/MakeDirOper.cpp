#include "FileExplorerAssertions.h"
#include "File.h"
#include "MessageTypes.h"
#include "MakeDirOper.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "UserManager.h"
#include "FileDBManager.h"
#include "ExcludeFilesFilter.h"

#include <vector>

namespace IBox
{

MakeDir::MakeDir(std::string operUser, std::string path) 
	: mOperUser(operUser)
	, mPath(path)
{
	
}

void MakeDir::start() 
{	
    mkDirs(mPath);
}

void MakeDir::mkDirs(std::string path) 
{
    File dir(path);
    if (!dir.exists()) {
        File parentDir(dir.getParent());
        if (!parentDir.exists())
            mkDirs(parentDir.path());
        if (dir.mkDir() && mFileChangedListener)
            mFileChangedListener->onDirCreated(mOperUser, dir.path());
    }
}

MakeDirOper::MakeDirOper(int code, std::string request) 
	: FileOper(code, request)
	, mResult(Result::OK)
{
	
}	

Result MakeDirOper::execute() 
{
	JsonObject requestObj(mOperRequest);
	
	std::string path = requestObj.getString("Path");
	std::string operUser = UserManager::getInstance()->getUserName(requestObj.getString("JID"));
	if (operUser.empty()) 
		operUser = "public";

	MakeDir mkdirs(operUser, path);
	mkdirs.setFileOperProgressListener(this);
	mkdirs.setFileChangedListener(FileDBManager::getInstance());
	mkdirs.start();
	return Result::OK;
}

void MakeDirOper::onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize)
{	
	
}	

MakeDefDirsOper::MakeDefDirsOper(std::string path) 
	: FileOper(0, "")
	, mTopPath(path)
{
	
}	

Result MakeDefDirsOper::execute() 
{	
	std::string operUser = "public";

	MakeDir mkBakDir(operUser, mTopPath + "/icebox/backup");
	mkBakDir.setFileChangedListener(FileDBManager::getInstance());
	mkBakDir.start();
	MakeDir mkDevDir(operUser, mTopPath + "/icebox/devices");
	mkDevDir.setFileChangedListener(FileDBManager::getInstance());
	mkDevDir.start();
	MakeDir mkUsersDir(operUser, mTopPath + "/icebox/users");
	mkUsersDir.setFileChangedListener(FileDBManager::getInstance());
	mkUsersDir.start();
	
	return Result::OK;
}

MakeUserDirsOper::MakeUserDirsOper(std::string username, std::string path) 
	: FileOper(0, "")
	, mOperUser(username)
	, mTopPath(path)
{
	
}	

Result MakeUserDirsOper::execute() 
{	
	if (mOperUser.empty()) 
		mOperUser = "public";

	MakeDir mkPicDir(mOperUser, mTopPath + "/Pictures");
	mkPicDir.setFileChangedListener(FileDBManager::getInstance());
	mkPicDir.start();
	MakeDir mkMusDir(mOperUser, mTopPath + "/Music");
	mkMusDir.setFileChangedListener(FileDBManager::getInstance());
	mkMusDir.start();
	MakeDir mkVedDir(mOperUser, mTopPath + "/Videos");
	mkVedDir.setFileChangedListener(FileDBManager::getInstance());
	mkVedDir.start();
	MakeDir mkDocDir(mOperUser, mTopPath + "/Documents");
	mkDocDir.setFileChangedListener(FileDBManager::getInstance());
	mkDocDir.start();
	
	return Result::OK;
}

} // namespace IBox



