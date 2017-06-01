#include "FileExplorerAssertions.h"
#include "FileDatabase.h"
#include "DateTime.h"
#include "File.h"

#include "MakeThumbnailOper.h"
#include "ExifReader.h"
#include "MediaInfo.h"
#include "DateTime.h"
#include "SplitString.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

namespace IBox {

FileDatabase::FileDatabase(std::string topDir) 
	: mTopDir(topDir)
{
	mDBFilePath = mTopDir + "/icebox/icebox.db";
	File dbFile(mDBFilePath);
	dbFile.mkParentDir();
	mSQLiteDB = new SQLiteDatabase();
    if (mSQLiteDB) {
        mSQLiteDB->open(mDBFilePath);
        mFileTable = new FileTable(mSQLiteDB);
        mFileTable->init();
        mFolderTable = new FolderTable(mSQLiteDB);
        mFolderTable->init();
		if (!mSQLiteDB->tableExists("status")) {
        	mSQLiteDB->exec("CREATE TABLE status(CreatedTime TEXT UNIQUE NOT NULL PRIMARY KEY, Ready INTEGER DEFAULT 0)");
			mSQLiteDB->exec("INSERT INTO status(CreatedTime) VALUES(?)", SQLText(DateTime::now().c_str()));
		}	
		pthread_mutex_init(&mMutex, NULL);
		std::string name = getFolderName(1);
		if (topDir.compare(name) != 0) {
			FILEEXPLORER_LOG("Change the top path to %s\n", topDir.c_str());
			updateFolder(1, "name", topDir);
		}	
    }
}

FileDatabase::~FileDatabase() {
	if (mFileTable != NULL) {
        delete mFileTable;
        mFileTable = NULL;
    }
    if (mFolderTable != NULL) {
        delete mFolderTable;
        mFolderTable = NULL;
    }
    if (mSQLiteDB != NULL) {
        delete mSQLiteDB;
        mSQLiteDB = NULL;
    }
}

void FileDatabase::setAlias(std::string alias) 
{
    FolderInfo info;
	info.name = alias;
	info.folderID = 0;

    if (getFolderName(info.folderID).empty())
        insertFolder(info, info.folderID);
}

std::string FileDatabase::getAlias() 
{
    return getFolderName(0);
}

int FileDatabase::getIndex() 
{
	int idx = 0;
	std::string alias = getAlias();
	if (!alias.empty())
		idx = atoi(alias.substr(alias.find_last_of(" ") + 1).c_str());
	return idx;
}

bool
FileDatabase::isReady()
{	
	int ready = 0;
	if (mSQLiteDB) {
		SQLiteResultSet* rs = mSQLiteDB->query("SELECT Ready FROM status");
	    if (rs) {
			if (rs->next())
	        	ready = rs->columnInt(0);
	        rs->close();
	    }
	}
	return ready;
}

void
FileDatabase::setReady()
{
	FILEEXPLORER_LOG("setReady\n");
	if (mSQLiteDB)
		mSQLiteDB->exec("UPDATE status SET Ready = 1");
}

void FileDatabase::insertFile(std::string owner, std::string path) {
    File file(path);
    FileInfo info;

    info.folderID = getFolderID(file.getParent());
    info.size = file.size();
    info.hash = ""; //file.hashCode();
    info.name = file.name();
    info.mtime = DateTime::strftime(file.lastModifiedTime(), "YYYY-MM-DD HH:MM:SS");
    info.ctime = DateTime::strftime(file.createdTime(), "YYYY-MM-DD HH:MM:SS");
    info.type = file.type();
    info.owner = owner;
    info.deviceName = "";
    info.stat = 0;
	info.stime = "";
	info.sdevice = "";
	info.backup = 1;
	info.encrypt = 0;
	
    if (!fileExists(info.name, info.folderID)) {
		if (file.type() == FileType_Image) {
			ThumbnailMaker::makeIcon(file.path());
			std::vector< std::pair<std::string, std::string> > exifData;
			if (getExifDataFromFile(file.path(), &exifData)) {
				std::vector< std::pair<std::string, std::string> >::iterator it;
				for (it = exifData.begin(); it < exifData.end(); it++) {
					if ((*it).first.compare("DateTime") == 0)
						info.stime = (*it).second;
//					else if ((*it).first.compare("Make") == 0)
//						make = (*it).second;
					else if ((*it).first.compare("Model") == 0)
						info.sdevice = (*it).second;
				}
			}
		} else if (file.type() == FileType_Audio) {
			struct MiAudioInfo stAudioInfo;
			MediaInfo mi;
    		mi.getInfo(file.path().c_str(), &stAudioInfo);
			info.album = stAudioInfo.mAlbum;
			info.artist = stAudioInfo.mPerformer;
			FILEEXPLORER_LOG("Album	: %s\n", stAudioInfo.mAlbum.c_str());
			FILEEXPLORER_LOG("Artist	: %s\n", stAudioInfo.mPerformer.c_str());
		}	
		insertFile(info);
    }	
}

void FileDatabase::insertFolder(std::string who, std::string path) 
{
    File dir(path);
    FolderInfo info;
	
    if (path.compare(mTopDir) == 0) {
        info.folder = 0;
        info.name = path;
    } else {
        info.folder = getFolderID(dir.getParent());
        info.name = dir.name();
    }
    info.mtime = DateTime::strftime(dir.lastModifiedTime(), "YYYY-MM-DD HH:MM:SS");
    info.ctime = DateTime::strftime(dir.createdTime(), "YYYY-MM-DD HH:MM:SS");
    info.size = 0;
    info.deviceName = "";
    info.stat = 0;
	info.owner = who;

    if (!folderExists(info.name, info.folder))
        insertFolder(info);
}

void FileDatabase::renameDir(std::string oldPath, std::string newPath) 
{
    File oldDir(oldPath);
    File newDir(newPath);
    FolderInfo info = getFolderInfo(oldDir.name(), getFolderID(oldDir.getParent()));
    if (info.folderID != 0) {
        info.name = newDir.name();
        info.folder = getFolderID(newDir.getParent());
        updateFolder(info);
    }
}

void FileDatabase::renameFile(std::string oldPath, std::string newPath) 
{
    File oldFile(oldPath);
    File newFile(newPath);
    FileInfo info = getFileInfo(oldFile.name(), getFolderID(oldFile.getParent()));
    if (info.fileID != 0) {
        info.name = newFile.name();
        info.folderID = getFolderID(newFile.getParent());
        updateFile(info);

		if (newFile.type() != FileType_Image)
			return ;

		File thumb(oldFile.path() + ".thumbnail");
		if (thumb.exists())
			thumb.renameTo(newFile.path() + ".thumbnail");
		File icon(oldFile.path() + ".hyicon");
		if (icon.exists())
			icon.renameTo(newFile.path() + ".hyicon");
		else
			ThumbnailMaker::makeIcon(newFile.path());
    }
}

void FileDatabase::recoverDir(std::string path) {
    File dir(path);
    recoverFolder(dir.name(), getFolderID(dir.getParent()));
}

void FileDatabase::recoverFile(std::string path) {
    File file(path);
    recoverFile(file.name(), getFolderID(file.getParent()));
}

void FileDatabase::removeDir(std::string path, bool realRemoveFile) {
    File dir(path);
    removeFolder(dir.name(), getFolderID(dir.getParent()), realRemoveFile);
}

void FileDatabase::removeFile(std::string path, bool realRemoveFile) {
    File file(path);
    removeFile(file.name(), getFolderID(file.getParent()), realRemoveFile);
}

void FileDatabase::updateSize(std::string path) {
    File file(path);
    if (file.isDirectory()) {
        FolderInfo info;
        if (path.compare(mTopDir) == 0)
            info = getFolderInfo(path, 0);
        else
            info = getFolderInfo(file.name(), getFolderID(file.getParent()));
        if (info.folderID != 0) {
            int size = getFolderSize(info.folderID);
			if (info.size != size) {
            	updateFolder(info.folderID, "size", size);
			}	
			if (path.compare(mTopDir) != 0) {
				updateSize(file.getParent());
			}	
        }
    } else {
        FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
        if (info.fileID != 0) {
            info.size = file.size();
            updateFile(info);
        }
    }
}

void FileDatabase::changeOwner(std::string toUser, std::string path) {
    File file(path);
    if (file.isDirectory()) {
        FolderInfo info;
        if (path.compare(mTopDir) == 0)
            info = getFolderInfo(path, 0);
        else
            info = getFolderInfo(file.name(), getFolderID(file.getParent()));
        if (info.folderID != 0 && info.chown.compare(toUser) != 0) 
            updateFolder(info.folderID, "chown", toUser);
    } else {
        FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
        if (info.fileID != 0 && info.chown.compare(toUser) != 0)
        	updateFile(info.fileID, "chown", toUser);
    }
}

std::string FileDatabase::getOwner(std::string path) {
	std::string owner;
	File file(path);
	if (file.isDirectory()) {
		FolderInfo info = getFolderInfo(file.name(), getFolderID(file.getParent()));
		if (info.folderID != 0) 
			owner = info.owner;
	} else {	
    	FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    	if (info.fileID != 0) 
			owner = info.owner;
	}	
	return owner;
}

void FileDatabase::setBackupFlag(std::string path)
{
	File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) 
        updateFile(info.fileID, "backup", 2);
}

int FileDatabase::getBackupFlag(std::string path)
{
	File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) 
        return info.backup;
}

int FileDatabase::getFolderID(std::string path) {
    int parentFolderID = 0;
    if (path.compare(mTopDir) == 0)
        return getFolderID(path, parentFolderID);
    else {
        size_t found = path.find_last_of("/");
        std::string parentDir = path.substr(0, found);
        std::string name = path.substr(found + 1);
        parentFolderID = getFolderID(parentDir);
        return getFolderID(name, parentFolderID);
    }
}

int FileDatabase::fileCount(std::string path, std::string owner, bool i) {
	File file(path);
    if (file.isDirectory()) {
		FolderInfo info;
		info = getFolderInfo(file.name(), getFolderID(file.getParent()));
		return getFileCount(info.folderID, owner, i);
    }
	return 0;
}

void FileDatabase::share(std::string owner, std::string to, std::string path) {
    File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        SplitString splitStr(info.shareWith, ",");
        splitStr.add(to);
		updateFile(info.fileID, "shareWith", splitStr.toString());
    }
}

void FileDatabase::unshare(std::string owner, std::string to, std::string path) {
    File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        SplitString splitStr(info.shareWith, ",");
        splitStr.erase(to);
		updateFile(info.fileID, "shareWith", splitStr.toString());
    }
}

void FileDatabase::setLabel(std::string who, std::string label, std::string path) {
    File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        updateFile(info.fileID, "labels", label);
    }
}

std::string FileDatabase::getFileHashCode(std::string path) {
	File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        return info.hash;
    }
	return "";
}

void FileDatabase::setFileHashCode(std::string path, std::string hash) {
	File file(path);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        updateFile(info.fileID, "hash", hash);
    }
}

void FileDatabase::encryptFile(std::string src, std::string dest) {
	File file(src);
	File newFile(dest);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        info.name = newFile.name();
		info.encrypt = 2;
        //info.folderID = getFolderID(newFile.getParent());
        updateFile(info);
    }
}

void FileDatabase::decryptFile(std::string src, std::string dest) {
	File file(src);
	File newFile(dest);
    FileInfo info = getFileInfo(file.name(), getFolderID(file.getParent()));
    if (info.fileID != 0) {
        info.name = newFile.name();
		info.encrypt = 0;
        //info.folderID = getFolderID(newFile.getParent());
        updateFile(info);
    }
}


//-----------------------------------------------------------------

bool 
FileDatabase::insertFile(FileInfo info) 
{
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->insert(info, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::insertFolder(FolderInfo info) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->insert(info, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::insertFolder(FolderInfo info, int folderID) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->insert(info, folderID, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFile(FileInfo info) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->update(info, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFolder(FolderInfo info) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->update(info, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFile(int id, std::string key, int value) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->update(id, key, value, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFolder(int id, std::string key, int value) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->update(id, key, value, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFile(int id, std::string key, std::string value) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->update(id, key, value, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::updateFolder(int id, std::string key, std::string value) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->update(id, key, value, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::recoverFile(std::string fileName, int folderID) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->recover(fileName, folderID, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::recoverFolder(std::string folderName, int folderID) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->recover(folderName, folderID, nextOpid());
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::removeFile(std::string fileName, int folderID, bool realRemoveFile) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->remove(fileName, folderID, nextOpid(), realRemoveFile);
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::removeFolder(std::string folderName, int folderID, bool realRemoveFile) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->remove(folderName, folderID, nextOpid(), realRemoveFile);
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::fileExists(std::string fileName, int folderID) {
	pthread_mutex_lock(&mMutex);
	bool result = mFileTable->fileExists(fileName, folderID);
	pthread_mutex_unlock(&mMutex);
	return result;
}

bool FileDatabase::folderExists(std::string folderName, int folderID) {
	pthread_mutex_lock(&mMutex);
	bool result = mFolderTable->folderExists(folderName, folderID);
	pthread_mutex_unlock(&mMutex);
	return result;
}

size_t FileDatabase::getFolderSize(int folderID) {
	pthread_mutex_lock(&mMutex);
	size_t size = mFolderTable->count(folderID) + mFileTable->count(folderID);
	pthread_mutex_unlock(&mMutex);
	return size;
}

std::string FileDatabase::getFolderName(int folderID) {
	pthread_mutex_lock(&mMutex);
	std::string result = mFolderTable->getNameByFolderID(folderID);
	pthread_mutex_unlock(&mMutex);
	return result;
}

int FileDatabase::getFolderID(std::string folderName, int parentFolderID) {
	pthread_mutex_lock(&mMutex);
	int id = mFolderTable->getFolderID(folderName, parentFolderID);
	pthread_mutex_unlock(&mMutex);
	return id;
}

FileInfo FileDatabase::getFileInfo(std::string fileName, int folderID) {
	pthread_mutex_lock(&mMutex);
	FileInfo info = mFileTable->get(fileName, folderID);
	pthread_mutex_unlock(&mMutex);
	return info;
}

FolderInfo FileDatabase::getFolderInfo(std::string folderName, int folderID) {
	pthread_mutex_lock(&mMutex);
	FolderInfo info = mFolderTable->get(folderName, folderID);
	pthread_mutex_unlock(&mMutex);
	return info;
}

int FileDatabase::getFileCount(int folderID, std::string owner, bool i) {
	pthread_mutex_lock(&mMutex);
	int count = mFolderTable->dirCount(folderID) 
				+ mFileTable->fileCount(folderID);
	pthread_mutex_unlock(&mMutex);
	return count;
}


std::string FileDatabase::getAbsolutePath(std::string name, int parentFolderID) 
{
	std::vector<std::string> dirs;

	int folderID = parentFolderID;
	dirs.push_back(name);
	
	while (folderID > 0) {
		FolderInfo info = mFolderTable->get(folderID);
		if (info.name.empty())
			break;
		folderID = info.folder;
		dirs.push_back(info.name);
	}

	std::string absPath;
	while (!dirs.empty()) {
		absPath.append(dirs.back() + "/");
		dirs.pop_back();
	}

	if (absPath[absPath.size() - 1] == '/')
		absPath.erase(absPath.end() - 1);

	return absPath;
}

void 
FileDatabase::listDeletedFiles(std::string owner, std::vector<std::string>& deletedFiles) 
{
	pthread_mutex_lock(&mMutex);
	std::vector<FileInfo> files;
	mFileTable->listDeletedFiles(owner, files);
	for (int i = 0; i < files.size(); i++) {
		std::string path = getAbsolutePath(files[i].name, files[i].folderID);
		if (!path.empty())
			deletedFiles.push_back(path);
	}
	std::vector<FolderInfo> folders;
	mFolderTable->listDeletedFolders(owner, folders);
	for (int i = 0; i < folders.size(); i++) {
		std::string path = getAbsolutePath(folders[i].name, folders[i].folder);
		if (!path.empty())
			deletedFiles.push_back(path);
	}
	pthread_mutex_unlock(&mMutex);
}

void 
FileDatabase::listNeedBackupFiles(std::list<std::string>& list) 
{
	pthread_mutex_lock(&mMutex);
	std::vector<FileInfo> files;
	mFileTable->listNeedBackupFiles(files);
	for (int i = 0; i < files.size(); i++) {
		std::string path = getAbsolutePath(files[i].name, files[i].folderID);
		if (!path.empty())
			list.push_back(path);
	}
	pthread_mutex_unlock(&mMutex);
}

std::string FileDatabase::diff(std::string user, int opID) {
	
	int index = 0;
	std::string dir = getFolderName(1);
	std::string path = dir + "/icebox/tmp";
	File tmpDir(path);
	tmpDir.mkDirs();
	pthread_mutex_lock(&mMutex);
	while (1) {
		char name[256] = {0};
		sprintf(name, "%s_%d_%d_diff.db", user.c_str(), opID, index);
    	path = dir + "/icebox/tmp/" + name;
		if (access(path.c_str(), F_OK) != 0)
			break;
		index++;
	}

    SQLiteDatabase diffDB;
    diffDB.open(path);
    diffDB.exec("ATTACH DATABASE ? as fullDB", SQLText(mDBFilePath.c_str()));

    FileTable files(&diffDB);
    files.init();
    SQLiteValue values[4];
    values[0] = SQLInt(opID);
    values[1] = SQLText(user.c_str());
    values[2] = SQLText("public");
	std::string match = "%" + user + "%";
    values[3] = SQLText(match.c_str());
    diffDB.exec("INSERT INTO files SELECT * FROM fullDB.files 	\
				WHERE fullDB.files.opid > ? AND (owner == ? OR owner == ? OR shareWith LIKE ?)",
                values, sizeof(values) / sizeof(values[0]));

    FolderTable folders(&diffDB);
    folders.init();
    diffDB.exec("INSERT INTO folders SELECT * FROM fullDB.folders \
				WHERE fullDB.folders.opid > ? AND (owner == ? OR owner == ?)",
                values, sizeof(values) / sizeof(values[0]));

	diffDB.exec("CREATE TABLE status(CreatedTime TEXT UNIQUE NOT NULL PRIMARY KEY, Ready INTEGER DEFAULT 0)");
	diffDB.exec("INSERT INTO status SELECT * FROM fullDB.status");
	
    diffDB.close();
	pthread_mutex_unlock(&mMutex);
    return path;
}


size_t FileDatabase::nextOpid() {
    size_t a = mFolderTable->max("opid");
    size_t b = mFileTable->max("opid");
    int max = a > b ? a : b;
    return max + 1;
}


} // namespace IBox


