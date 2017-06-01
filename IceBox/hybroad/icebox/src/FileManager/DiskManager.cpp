#include "FileExplorerAssertions.h"
#include "DiskManager.h"
#include "StringData.h"
#include "JsonObject.h"
#include "JsonObjectArray.h"
#include "MessageTypes.h"
#include "FileDBManager.h"
#include "File.h"
#include "StringData.h"
#include "NativeHandler.h"
#include "CopyOper.h"
#include "MakeDirOper.h"
#include "FileOperThreadManager.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/statfs.h>

namespace IBox
{

DiskMngMsgHandler::DiskMngMsgHandler()
{
}

void DiskMngMsgHandler::handleMessage(Message* msg)
{
    if (msg) {
        StringData* strData = dynamic_cast<StringData*>(msg->obj);
        if (strData) {
            const char* jsonStr = strData->getData();

            if (!isJsonString(jsonStr))
                return ;

            switch (msg->what) {
            case MV_DATABASE_ADDDISK:
                DiskManager::getInstance()->onDiskAdded(jsonStr);
                return;
            case MV_DATABASE_REMOVEDISK:
                DiskManager::getInstance()->onDiskRemoved(jsonStr);
                return;
            default:
                break;
            }
        }
    }
}


DiskManager::DiskManager()
    : mMsgHandler(NULL)
{
    mDBFilePath = "/root/database/partitions.db";
    File dbFile(mDBFilePath);
    if (!dbFile.exists())
        dbFile.mkParentDir();
    mSQLiteDB = new SQLiteDatabase();
    if (mSQLiteDB) {
        mSQLiteDB->open(mDBFilePath);
        mDiskTable = new DiskTable(mSQLiteDB);
        mDiskTable->init();
        pthread_mutex_init(&mMutex, NULL);
    }
}

DiskManager::~DiskManager()
{
    if (mMsgHandler != NULL) {
        delete mMsgHandler;
        mMsgHandler = NULL;
    }
    if (mDiskTable != NULL) {
        delete mDiskTable;
        mDiskTable = NULL;
    }
    if (mSQLiteDB != NULL) {
        delete mSQLiteDB;
        mSQLiteDB = NULL;
    }
}

void
DiskManager::run()
{
    if (mMsgHandler == NULL) {
        mMsgHandler = new DiskMngMsgHandler();
    }
    Thread::run();
}

void
DiskManager::sendMessage(int what, int arg1, int arg2, const char* data)
{
    StringData* s = new StringData(data);
    Message* m = mMsgHandler->obtainMessage(what, arg1, arg2, s);
    mMsgHandler->sendMessage(m);
    if (s != NULL)
        s->safeUnref();
}

void DiskManager::getDiskSpaceSize(std::string dir, size_t& total, size_t& free)
{
    struct statfs s;
    if (statfs(dir.c_str(), &s) == 0) {
        total = (size_t)((long long)(s.f_blocks - s.f_bfree + s.f_bavail) * s.f_bsize / 1024 / 1024);
        free = (size_t)((long long)s.f_bavail * s.f_bsize / 1024 / 1024);
    }
}

DiskInfo DiskManager::getDiskInfo(VDisk* vd)
{
    std::string dir = vd->getTopDir();
    DiskInfo info;
    info.id = 0;
    info.type = "";
    info.fsname = vd->getDevName();
    info.mntdir = dir;
    info.bustype = "";
    info.alias = FileDBManager::getInstance()->getDiskAlias(dir);
    info.backupId = 0;
    getDiskSpaceSize(dir, info.total, info.free);
    return info;
}

void DiskManager::insertDisk(VDisk* vd)
{
    if (vd == NULL || mDiskTable == NULL)
        return ;

    DiskInfo info = getDiskInfo(vd);
    if (info.alias.empty()) {
        char alias[256] = {0};
        sprintf(alias, "%s %d", vd->diskName().c_str(), mDiskTable->nextId());
        logMsg("vd->getTopDir() = %s, alias = %s\n", vd->getTopDir().c_str(), alias);
        FileDBManager::getInstance()->setDiskAlias(vd->getTopDir(), alias);
        info.alias = alias;
        mDiskTable->insert(info);
    }
}

std::string DiskManager::getDiskDBFile()
{
    std::string path = "/root/database";
    File dir(path);
    dir.mkDirs();
    path.append("/disks.db");
    unlink(path.c_str());

    SQLiteDatabase diskDB;
    diskDB.open(path);
    DiskTable diskTable(&diskDB);
    diskTable.init();

    std::vector<LocalDisk*>::iterator it;

    for (int i = 0; i < mLocalDiskArray.size(); i++) {
        if (FileDBManager::getInstance()->diskIsReady(mLocalDiskArray[i]->getTopDir())) {
            DiskInfo info = getDiskInfo(mLocalDiskArray[i]);
            diskTable.insert(info);
        }
    }
	for (int i = 0; i < mBackupDiskArray.size(); i++) {
        if (FileDBManager::getInstance()->diskIsReady(mBackupDiskArray[i]->getTopDir())) {
			File dir(mBackupDiskArray[i]->getTopDir());
			int backupID = atoi(dir.name().c_str());
			if (backupID <= 0)
				continue;
			bool found = false;
			for (int j = 0; j < mLocalDiskArray.size(); j++) {
		        if (FileDBManager::getInstance()->getDiskIndex(mLocalDiskArray[j]->getTopDir()) == backupID) {
		            found = true;
					break;
		        }
		    }
			if (!found) {
            	DiskInfo info = getDiskInfo(mBackupDiskArray[i]);
				info.alias = mDiskTable->get(backupID).alias;
            	diskTable.insert(info);
			}
        }
    }
    for (int i = 0; i < mRemovableDiskArray.size(); i++) {
        if (FileDBManager::getInstance()->diskIsReady(mRemovableDiskArray[i]->getTopDir())) {
            DiskInfo info = getDiskInfo(mRemovableDiskArray[i]);
            diskTable.insert(info);
        }
    }

    diskDB.close();
    return path;
}

LocalDisk* DiskManager::addLocalDisk(std::string devName, std::string dir)
{
    std::vector<LocalDisk*>::iterator it;

    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0)
            return *it;
    }

    LocalDisk* disk = new LocalDisk(dir);
    if (disk) {
        disk->setDevName(devName);
        mLocalDiskArray.push_back(disk);
        FileDBManager::getInstance()->onDiskMounted(disk);
        insertDisk(disk);
		MakeDefDirsOper* oper = new MakeDefDirsOper(disk->getTopDir());
		FileOperThreadManager::getInstance()->execute(oper);
        return disk;
    }

    return NULL;
}

void DiskManager::removeLocalDisk(std::string devName, std::string dir)
{
    std::vector<LocalDisk*>::iterator it;

    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            mLocalDiskArray.erase(it);
            return ;
        }
    }
}

void DiskManager::removeLocalDisk(std::string devName)
{
    std::vector<LocalDisk*>::iterator it;

	it = mLocalDiskArray.begin();
	while (it != mLocalDiskArray.end()) {
        if ((*it)->getDevName().compare(0, devName.size(), devName) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            it = mLocalDiskArray.erase(it);
        } else it++;
    }
}

RemovableDisk* DiskManager::addRemovableDisk(std::string devName, std::string dir)
{
    std::vector<RemovableDisk*>::iterator it;

    for (it = mRemovableDiskArray.begin(); it != mRemovableDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0)
            return *it;
    }

    RemovableDisk* disk = new RemovableDisk(dir);
    if (disk) {
        disk->setDevName(devName);
        mRemovableDiskArray.push_back(disk);
        FileDBManager::getInstance()->onDiskMounted(disk);
        insertDisk(disk);
        return disk;
    }

    return NULL;
}

void DiskManager::removeRemovableDisk(std::string devName, std::string dir)
{
    std::vector<RemovableDisk*>::iterator it;

    for (it = mRemovableDiskArray.begin(); it != mRemovableDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            mRemovableDiskArray.erase(it);
            return ;
        }
    }
}

void DiskManager::removeRemovableDisk(std::string devName)
{
    std::vector<RemovableDisk*>::iterator it;

	it = mRemovableDiskArray.begin();
	while (it != mRemovableDiskArray.end()) {
		if ((*it)->getDevName().compare(0, devName.size(), devName) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            it = mRemovableDiskArray.erase(it);
        } else it++;
	}
}

BackupDisk* DiskManager::addBackupDisk(std::string devName, std::string dir)
{
    std::vector<BackupDisk*>::iterator it;

    for (it = mBackupDiskArray.begin(); it != mBackupDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0)
            return *it;
    }

    BackupDisk* disk = new BackupDisk(dir);
    if (disk) {
        disk->setDevName(devName);
        mBackupDiskArray.push_back(disk);
        FileDBManager::getInstance()->onDiskMounted(disk);
        insertDisk(disk);
        return disk;
    }

    return NULL;
}

void DiskManager::removeBackupDisk(std::string devName, std::string dir)
{
    std::vector<BackupDisk*>::iterator it;

    for (it = mBackupDiskArray.begin(); it != mBackupDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            mBackupDiskArray.erase(it);
            return ;
        }
    }
}

void DiskManager::removeBackupDisk(std::string devName)
{
    std::vector<BackupDisk*>::iterator it;

	it = mBackupDiskArray.begin();
	while (it != mBackupDiskArray.end()) {
        if ((*it)->getDevName().compare(0, devName.size(), devName) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            it = mBackupDiskArray.erase(it);
        } else it++;
    }
}

CopyDisk* DiskManager::addCopyDisk(std::string devName, std::string dir)
{
    std::vector<CopyDisk*>::iterator it;

    for (it = mCopyDiskArray.begin(); it != mCopyDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0)
            return *it;
    }

    CopyDisk* disk = new CopyDisk(dir);
    if (disk) {
        disk->setDevName(devName);
        mCopyDiskArray.push_back(disk);
        return disk;
    }

    return NULL;
}

void DiskManager::removeCopyDisk(std::string devName, std::string dir)
{
    std::vector<CopyDisk*>::iterator it;

    for (it = mCopyDiskArray.begin(); it != mCopyDiskArray.end(); it++) {
        if ((*it)->getTopDir().compare(dir) == 0) {
            delete(*it);
            mCopyDiskArray.erase(it);
            return ;
        }
    }
}

void DiskManager::removeCopyDisk(std::string devName)
{
    std::vector<CopyDisk*>::iterator it;

	it = mCopyDiskArray.begin();
    while (it != mCopyDiskArray.end()) {
        if ((*it)->getDevName().compare(0, devName.size(), devName) == 0) {
            FileDBManager::getInstance()->onDiskUMounted(*it);
            delete(*it);
            it = mCopyDiskArray.erase(it);
        } else it++;
    }
}

std::string DiskManager::getLocalDiskTopDir(int index)
{
	if (index < 0 || index >= mLocalDiskArray.size())
		return "";
	return mLocalDiskArray[index]->getTopDir();
}

int DiskManager::localDiskCount()
{
	return mLocalDiskArray.size();
}

int DiskManager::localDiskDeviceCount()
{
	std::vector<std::string> diskDevs;

	std::vector<LocalDisk*>::iterator it;
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
		int i = 0;
		for (i = 0; i < diskDevs.size(); i++) {
			if ((*it)->getDevName().compare(0, 3, diskDevs[i]) == 0)
				break;
		}
		if (i == diskDevs.size())
			diskDevs.push_back((*it)->getDevName().substr(0, 3));
    }

	return diskDevs.size();
}

std::string DiskManager::getOneKeyCopyDir(std::string sn)
{
    std::vector<LocalDisk*>::iterator it;
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        File dir((*it)->getOneKeyCopyDir() + "/" + sn);
        if (dir.exists())
            return dir.path();
    }

    size_t max = 0;
    std::string path = "";
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        size_t total = 0;
        size_t free = 0;
        getDiskSpaceSize((*it)->getTopDir(), total, free);
        if (free > max) {
            max = free;
            path = (*it)->getOneKeyCopyDir();
        }
    }
    if (!path.empty()) {
        path.append("/" + sn);
        //File dir(path);
        //dir.mkDirs();
    }
    return path;
}

std::string DiskManager::getUserDir(std::string username)
{
	if (mLocalDiskArray.size() == 0)
		return "";
	
    std::vector<LocalDisk*>::iterator it;
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        File dir((*it)->getUserDir() + "/" + username);
        if (dir.exists())
            return dir.path();
    }

    it = mLocalDiskArray.begin();
	return (*it)->getUserDir() + "/" + username;
}

std::string DiskManager::getBackupDir(std::string topDir)
{
	int index = FileDBManager::getInstance()->getDiskIndex(topDir);
	if (index <= 0)
		return "";
	char dirName[128] = {0};
	sprintf(dirName, "%d", index);
	
    std::vector<BackupDisk*>::iterator it;
    for (it = mBackupDiskArray.begin(); it != mBackupDiskArray.end(); it++) {
		File dir((*it)->getTopDir());
        if (dir.name().compare(dirName) == 0)
			return (*it)->getTopDir();
    }

	std::string diskDeviceName = "";
	std::vector<LocalDisk*>::iterator iter;
    for (iter = mLocalDiskArray.begin(); iter != mLocalDiskArray.end(); iter++) {
		if ((*iter)->getTopDir().compare(topDir) == 0) {
			diskDeviceName = (*iter)->getDevName().substr(0, 3);
			break;
		}	
	}

	if (diskDeviceName.empty()) {
		logMsg("No such disk %s.\n", topDir.c_str());
		return "";
	}
		
    size_t max = 0;
    std::string path = "";
	std::string devName = "";
    for (iter = mLocalDiskArray.begin(); iter != mLocalDiskArray.end(); iter++) {
		if ((*iter)->getDevName().compare(0, diskDeviceName.size(), diskDeviceName) == 0)
			continue;
        size_t total = 0;
        size_t free = 0;
        getDiskSpaceSize((*iter)->getTopDir(), total, free);
        if (free > max) {
            max = free;
            path = (*iter)->getBackupDir();
			devName = (*iter)->getDevName();
        }
    }
	
    if (!path.empty()) {
        path.append("/"); 
		path.append(dirName);
        File dir(path);
        dir.mkDirs();
		addBackupDisk(devName, path);
    }
    return path;
}

int DiskManager::getPartitionNumber(std::string devName)
{
    int count = 0;

    std::vector<LocalDisk*>::iterator it;
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        if ((*it)->getDevName().size() > devName.size()
            && (*it)->getDevName().compare(0, devName.size(), devName) == 0)
            count ++;
    }

    if (count > 0)
        return count;

    std::vector<RemovableDisk*>::iterator iter;
    for (iter = mRemovableDiskArray.begin(); iter != mRemovableDiskArray.end(); iter++) {
        if ((*iter)->getDevName().size() > devName.size()
            && (*iter)->getDevName().compare(0, devName.size(), devName) == 0)
            count ++;
    }

    return count;
}

std::string DiskManager::getDiskTopDir(std::string devName, int partitionIdx)
{
	int count = 0;

    std::vector<LocalDisk*>::iterator it;
    for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
        if ((*it)->getDevName().size() > devName.size()
            && (*it)->getDevName().compare(0, devName.size(), devName) == 0) {
			if (count == partitionIdx)
				return (*it)->getTopDir();
			count ++;
        }	
    }

    std::vector<RemovableDisk*>::iterator iter;
    for (iter = mRemovableDiskArray.begin(); iter != mRemovableDiskArray.end(); iter++) {
        if ((*iter)->getDevName().size() > devName.size()
            && (*iter)->getDevName().compare(0, devName.size(), devName) == 0) {
			if (count == partitionIdx)
				return (*iter)->getTopDir();
			count ++;
        }	
    }

    return "";
}

void DiskManager::onDiskMounted(std::string busType, std::string jsonStr)
{
    logMsg("onDiskMounted, %s\n", jsonStr.c_str());
    JsonObject  jsonObj(jsonStr);
    std::string mntDir = jsonObj.getString("MountDir");
    std::string devName = jsonObj.getString("PartName");

    if (busType.compare("ATA") == 0) {
        LocalDisk* localDisk = addLocalDisk(devName, mntDir);
        if (localDisk) {
            std::vector<std::string> dirs = localDisk->listBackupDirs();
            for (int i = 0; i < dirs.size(); i++) {
                addBackupDisk(devName, dirs[i]);
            }

            dirs = localDisk->listCopyDirs();
            for (int i = 0; i < dirs.size(); i++) {
                addCopyDisk(devName, dirs[i]);
            }
        }
    } else if (busType.compare("USB") == 0 || busType.compare("SDC") == 0) {
        RemovableDisk* removableDisk = addRemovableDisk(devName, mntDir);
    }
    dbg_print();
}

void DiskManager::onDiskAdded(std::string jsonStr)
{
    logMsg("onDiskAdded, %s\n", jsonStr.c_str());
    JsonObject  jsonObj(jsonStr);
    std::string devName = jsonObj.getString("Disk");
	std::string busType = jsonObj.getString("BusType");
    int partitionsCount = jsonObj.getInt("PartitionsCount");
	JsonObjectArray partitions(jsonObj.getString("Partitions"));
    for (int i = 0; i < partitionsCount; i++) {
		onDiskMounted(busType, partitions.get(i).toString());
	}
    dbg_print();
	if (busType.compare("ATA")) {
		StringData* strData = new StringData(devName.c_str());
		NativeHandler& H = defNativeHandler();
		H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_RECEIVE, 0, strData));
		strData->safeUnref();	
	}
}

void DiskManager::onDiskRemoved(std::string jsonStr)
{
    logMsg("onDiskRemoved, %s\n", jsonStr.c_str());
    JsonObject  jsonObj(jsonStr);
    std::string devName = jsonObj.getString("Disk");
    std::string type = jsonObj.getString("BusType");

    if (type.compare("ATA") == 0) {
        removeBackupDisk(devName);
        removeCopyDisk(devName);
        removeLocalDisk(devName);
    } else if (type.compare("USB") == 0 || type.compare("SDC") == 0) {
        removeRemovableDisk(devName);
    }
    dbg_print();
}

void DiskManager::dbg_print()
{
    {
        logMsg("LocalDisk: %d\n", mLocalDiskArray.size());
        std::vector<LocalDisk*>::iterator it;
        for (it = mLocalDiskArray.begin(); it != mLocalDiskArray.end(); it++) {
            logMsg("\t%s\t%s\n", (*it)->getTopDir().c_str(), (*it)->getDevName().c_str());
        }
    }
    {
        logMsg("RemovableDisk: %d\n", mRemovableDiskArray.size());
        std::vector<RemovableDisk*>::iterator it;
        for (it = mRemovableDiskArray.begin(); it != mRemovableDiskArray.end(); it++) {
            logMsg("\t%s\t%s\n", (*it)->getTopDir().c_str(), (*it)->getDevName().c_str());
        }
    }
    {
        logMsg("BackupDisk: %d\n", mBackupDiskArray.size());
        std::vector<BackupDisk*>::iterator it;
        for (it = mBackupDiskArray.begin(); it != mBackupDiskArray.end(); it++) {
            logMsg("\t%s\t%s\n", (*it)->getTopDir().c_str(), (*it)->getDevName().c_str());
        }
    }
    {
        logMsg("CopyDisk: %d\n", mCopyDiskArray.size());
        std::vector<CopyDisk*>::iterator it;
        for (it = mCopyDiskArray.begin(); it != mCopyDiskArray.end(); it++) {
            logMsg("\t%s\t%s\n", (*it)->getTopDir().c_str(), (*it)->getDevName().c_str());
        }
    }
}

Result DiskManager::handleRequest(int code, std::string& request)
{
	logMsg("Code: 0x%x, Request: %s.\n", code, request.c_str());
	switch(code) {
	case MV_DATABASE_REMOVEDISK:
	case MV_DATABASE_ADDDISK:	
		sendMessage(code, 0, 0, request.c_str());
		return Result::OK;	
	}
	if (!isJsonString(request.c_str()))
		return Result::NoHandle;
	
	JsonObject jsonObj(request);
	if (code == MV_DISK_COPY) {
		std::string diskName = jsonObj.getString("Disk");
	    std::string serialNum = jsonObj.getString("SerialNumber");
		CopyDiskOper* oper = new CopyDiskOper(diskName, getOneKeyCopyDir(serialNum));
		FileOperThreadManager::getInstance()->execute(oper);
		return Result::OK;	
	} else if (jsonObj.getString("Command").compare("Refresh") == 0
		&& jsonObj.getString("Path").compare("/") == 0) {
		JsonObject response(request);
		response.add("Action", "FunctionResponse");
		std::string DBPath = getDiskDBFile();
		if (!DBPath.empty())
        response.add("Path", DBPath);
    	response.add("Response", DBPath.empty() ? "Failed" : "Success");

		StringData* strData = new StringData(response.toString().c_str());
		NativeHandler& H = defNativeHandler();
		H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_RESPONSE, code, strData));
		strData->safeUnref();
		return Result::OK;	
	}
				
	return Result::NoHandle;
}

void DiskManager::response(int code, std::string request, Result result)
{
	
}	

static DiskManager* gDiskManager = NULL;

DiskManager* DiskManager::getInstance()
{
    if (gDiskManager == NULL) {
        gDiskManager = new DiskManager();
		gDiskManager->start();
	}	
    return gDiskManager;
}

void DiskManager::releaseInstance()
{
    if (gDiskManager) {
        delete gDiskManager;
        gDiskManager = NULL;
    }
}

void DiskMngInit()
{
    DiskManager::getInstance()->start();
}

} /* namespace IBox */
