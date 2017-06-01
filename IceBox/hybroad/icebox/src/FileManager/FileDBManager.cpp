#include "FileExplorerAssertions.h"
#include "FileDBManager.h"
#include "File.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "ExcludeFilesFilter.h"
#include "ScanOper.h"
#include "UserManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace IBox
{

FileDBManager::FileDBManager()
{

}

FileDBManager::~FileDBManager()
{

}

void FileDBManager::setLabel(std::string username, std::string label, std::string path)
{
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        db->setLabel(username, label, path);
    }
}

std::string FileDBManager::diffDatabase(std::string user, int opid, std::string path)
{
    FileDatabase* db = getFileDatabase(path);
    return (db ? db->diff(user, opid) : "");
}

FileDatabase* FileDBManager::addFileDatabase(std::string dir)
{
    std::vector<FileDatabase*>::iterator it;
	
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0)
            return *it;
    }

    FileDatabase* db = new FileDatabase(dir);
    if (db) {
        mFileDBArray.push_back(db);
        return db;
    }

    return NULL;
}

void FileDBManager::removeFileDatabase(std::string dir)
{
    std::vector<FileDatabase*>::iterator it;

    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            delete(*it);
            mFileDBArray.erase(it);
			return ;
        }
    }
}

FileDatabase* FileDBManager::getFileDatabase(std::string path)
{
	int matchLen = 0;
	FileDatabase* db = NULL;
    std::vector<FileDatabase*>::iterator it;

    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if (path.size() >= (*it)->getTopDir().size()
            && path.compare(0, (*it)->getTopDir().size(), (*it)->getTopDir()) == 0
            && (*it)->getTopDir().size() > matchLen)
            db = *it;
    }
    return db;
}

int FileDBManager::getDiskIndex(std::string dir)
{
    std::vector<FileDatabase*>::iterator it;
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            return (*it)->getIndex();
        }
    }

    return -1;
}

std::string FileDBManager::getDiskAlias(std::string dir)
{
    std::vector<FileDatabase*>::iterator it;
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            logMsg("Alias : %s\n", (*it)->getAlias().c_str());
            return (*it)->getAlias();
        }
    }

    return "";
}

void FileDBManager::setDiskAlias(std::string dir, std::string alias)
{
    std::vector<FileDatabase*>::iterator it;
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
		logMsg("(*it)->getTopDir() = %s, dir = %s\n", (*it)->getTopDir().c_str(), dir.c_str());
        if ((*it)->getTopDir().compare(dir) == 0)
            (*it)->setAlias(alias);
    }
}

bool FileDBManager::diskIsReady(std::string dir)
{
    std::vector<FileDatabase*>::iterator it;
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0
            && (*it)->isReady())
            return true;
    }
    return false;
}

void FileDBManager::onDiskMounted(VDisk* vd)
{
    if (vd == NULL)
        return ;

	
    FileDatabase* fileDB = addFileDatabase(vd->getTopDir());
	if (fileDB == NULL)
		return ;

	
	if (vd->isRemovableDisk()) {
		std::string alias = fileDB->getAlias();
		std::string dbPath = fileDB->getDBFilePath();
		removeFileDatabase(vd->getTopDir());
		File dbFile(dbPath);
		dbFile.remove();
		fileDB = addFileDatabase(vd->getTopDir());
		if (fileDB)
			fileDB->setAlias(alias);
	}
		
	if (fileDB && !fileDB->isReady()) {
        ScanDisk scan(fileDB->getTopDir());
		scan.start();
        fileDB->setReady();
    }
}

void FileDBManager::onDiskUMounted(VDisk* vd)
{
    if (vd)
        removeFileDatabase(vd->getTopDir());
}

void FileDBManager::onFileCreated(std::string user, std::string path)
{
    logMsg("onFileCreated: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db)
        db->insertFile(user, path);
}

void FileDBManager::onDirCreated(std::string user, std::string path)
{
    logMsg("onDirCreated: %s\n", path.c_str(), user.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db)
        db->insertFolder(user, path);
}

void FileDBManager::onFileRenamed(std::string oldPath, std::string newPath)
{
    logMsg("onFileRenamed: old file: %s, new file: %s\n", oldPath.c_str(), newPath.c_str());
    FileDatabase* src = getFileDatabase(oldPath);
    FileDatabase* dest = getFileDatabase(newPath);
    if (src && dest) {
        if (src == dest)
            src->renameFile(oldPath, newPath);
        else {
            dest->insertFile(src->getOwner(oldPath), newPath);
            src->removeFile(oldPath, true);
        }
    }
}

void FileDBManager::onDirRenamed(std::string oldPath, std::string newPath)
{
    logMsg("onDirRenamed: old file: %s, new file: %s\n", oldPath.c_str(), newPath.c_str());
    FileDatabase* src = getFileDatabase(oldPath);
    FileDatabase* dest = getFileDatabase(newPath);
    if (src && dest) {
        if (src == dest)
            src->renameDir(oldPath, newPath);
        else {
            dest->insertFolder(src->getOwner(oldPath), newPath);
            src->removeDir(oldPath, true);
        }
    }
}

void FileDBManager::onFileDeleted(std::string user, std::string path)
{
    logMsg("onFileDeleted: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if (owner.compare("public") == 0 || owner.compare(user) == 0)
            db->removeFile(path, false);
    }
}

void FileDBManager::onDirDeleted(std::string user, std::string path)
{
    logMsg("onDirDeleted: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if ((owner.compare("public") == 0 || owner.compare(user) == 0)
            && db->fileCount(path, user, false) == db->fileCount(path, "public", true)) {
            db->removeDir(path, false);
        }
    }
}

void FileDBManager::onFileRecovered(std::string user, std::string path)
{
    logMsg("onFileRecovered: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if (owner.compare("public") == 0 || owner.compare(user) == 0)
            db->recoverFile(path);
    }
}

void FileDBManager::onDirRecovered(std::string user, std::string path)
{
    logMsg("onDirRecovered: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if (owner.compare("public") == 0 || owner.compare(user) == 0)
            db->recoverDir(path);
    }
}

void FileDBManager::onFileRealDeleted(std::string user, std::string path)
{
    logMsg("onFileRealDeleted: %s\n", path.c_str());
    File file(path);
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if ((owner.empty() || owner.compare("public") == 0 || owner.compare(user) == 0)
            && file.remove()) {
            db->removeFile(path, true);
        }
    }
}

void FileDBManager::onDirRealDeleted(std::string user, std::string path)
{
    logMsg("onDirRealDeleted: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if ((owner.empty()
             || ((owner.compare("public") == 0 || owner.compare(user) == 0)
                 && db->fileCount(path, user, false) == db->fileCount(path, "public", true)))
            && rmdir(path.c_str()) == 0) {
            db->removeDir(path, true);
        }
    }
}

void FileDBManager::onSizeChanged(std::string path)
{
    logMsg("onSizeChanged: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        db->updateSize(path);
    }
}

void FileDBManager::onOwnerChanged(std::string user, std::string toUser, std::string path)
{
    logMsg("onOwnerChanged: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        std::string owner = db->getOwner(path).c_str();
        if ((owner.empty()
             || ((owner.compare("public") == 0 || owner.compare(user) == 0)
                 && db->fileCount(path, user, false) == db->fileCount(path, "public", true)))) {
            db->changeOwner(toUser, path);
        }
    }
}

void FileDBManager::onShareChanged(std::string user, std::string toUser, std::string path, bool shared)
{
    logMsg("onShareChanged: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        shared ? db->share(user, toUser, path) : db->unshare(user, toUser, path);
    }
}

void FileDBManager::onFileBackuped(std::string src, std::string dest)
{
    logMsg("onFileBackuped: %s\n", src.c_str());
    FileDatabase* srcDB = getFileDatabase(src);
	FileDatabase* destDB = getFileDatabase(dest);
    if (srcDB && destDB) {
		srcDB->setBackupFlag(src);
        destDB->insertFile(srcDB->getOwner(src), dest);
    }
}

void FileDBManager::onDirBackuped(std::string src, std::string dest)
{
    logMsg("onDirBackuped: %s\n", src.c_str());
    FileDatabase* srcDB = getFileDatabase(src);
	FileDatabase* destDB = getFileDatabase(dest);
    if (srcDB && destDB) {
        destDB->insertFolder(srcDB->getOwner(src), dest);
    }
}

bool FileDBManager::checkAccess(std::string user, std::string path)
{
	FileDatabase* db = getFileDatabase(path);
    if (db) {
		std::string owner = db->getOwner(path).c_str();
		if (owner.empty()
			|| owner.compare("public") == 0
			|| owner.compare(user) == 0)
			return true;
    }
	return false;
}

bool FileDBManager::checkDelete(std::string user, std::string path)
{
	FileDatabase* db = getFileDatabase(path);
    if (db) {
		File file(path);
		std::string owner = db->getOwner(path).c_str();
		if (file.isFile()) {
			if (owner.empty()
				|| owner.compare("public") == 0
				|| owner.compare(user) == 0)
				return true;
		} else if (file.isDirectory()) {
			if ((owner.empty()
				|| owner.compare("public") == 0
				|| owner.compare(user) == 0)
				&& db->fileCount(path, user, false) == db->fileCount(path, "public", true))
				return true;
		}
    }
	return false;
}


bool FileDBManager::needBackup(std::string path)
{
	FileDatabase* db = getFileDatabase(path);
    if (db) {
		int flag = db->getBackupFlag(path);
		if (flag == 1)
			return true;
    }
	return false;
}

void FileDBManager::onHashCodeChanged(std::string path, std::string hash)
{
    logMsg("onHashCodeChanged: %s\n", path.c_str());
    FileDatabase* db = getFileDatabase(path);
    if (db) {
        db->setFileHashCode(path, hash);
    }
}

void FileDBManager::onFileEncrypted(std::string src, std::string dest)
{
    logMsg("onFileEncrypted: src: %s, dest: %s\n", src.c_str(), dest.c_str());
    FileDatabase* db = getFileDatabase(src);
    if (db) {
        db->encryptFile(src, dest);
    }
}

void FileDBManager::onFileDecrypted(std::string src, std::string dest)
{
    logMsg("onFileDecrypted: src: %s, dest: %s\n", src.c_str(), dest.c_str());
    FileDatabase* db = getFileDatabase(src);
    if (db) {
        db->decryptFile(src, dest);
    }
}

std::string FileDBManager::getFileHashCode(std::string path)
{
	FileDatabase* db = getFileDatabase(path);
	return db ? db->getFileHashCode(path) : "";
}

void FileDBManager::listDeletedFiles(std::string user, std::vector<std::string>& deteledFiles)
{
	std::vector<FileDatabase*>::iterator it;
    for (it = mFileDBArray.begin(); it != mFileDBArray.end(); it++) {
		(*it)->listDeletedFiles(user, deteledFiles);
    }
}

static FileDBManager* gFileDBManager = NULL;

FileDBManager* FileDBManager::getInstance()
{
    if (gFileDBManager == NULL) {
        gFileDBManager = new FileDBManager();
		//gFileDBManager->init();
    }
    return gFileDBManager;
}

void FileDBManager::releaseInstance()
{
    if (gFileDBManager) {
        delete gFileDBManager;
        gFileDBManager = NULL;
    }
}

} /* namespace IBox */
