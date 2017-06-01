#ifndef __DiskMng__H_
#define __DiskMng__H_

#include "Handler.h"
#include "Result.h"

#include "DiskTable.h"
#include "LocalDisk.h"
#include "RemovableDisk.h"
#include "BackupDisk.h"
#include "CopyDisk.h"
#include "FileDatabase.h"
#include "Thread.h"
#include "Message.h"
#include "MessageHandler.h"

namespace IBox {

class DiskMngMsgHandler : public MessageHandler {
public:
	DiskMngMsgHandler();
	void handleMessage(Message* msg);
};

class DiskManager : public Thread, public Handler {
public:
	~DiskManager();
	static DiskManager* getInstance(); 
	static void releaseInstance();

	int localDiskCount();
	std::string getLocalDiskTopDir(int index);
	LocalDisk* addLocalDisk(std::string devName, std::string dir);
	void removeLocalDisk(std::string devName, std::string dir);
	void removeLocalDisk(std::string devName);	
	RemovableDisk* addRemovableDisk(std::string devName, std::string dir);
	void removeRemovableDisk(std::string devName, std::string dir);
	void removeRemovableDisk(std::string devName);		
	BackupDisk* addBackupDisk(std::string devName, std::string dir);
	void removeBackupDisk(std::string devName, std::string dir);
	void removeBackupDisk(std::string devName);		
	CopyDisk* addCopyDisk(std::string devName, std::string dir);
	void removeCopyDisk(std::string devName, std::string dir);
	void removeCopyDisk(std::string devName);		
									
	void onDiskMounted(std::string busType, std::string jsonStr);
	void onDiskAdded(std::string jsonStr);
	void onDiskRemoved(std::string jsonStr);
	void dbg_print();
	void run();
	void sendMessage(int what, int arg1, int arg2, const char* data);
	void getDiskSpaceSize(std::string dir, size_t& total, size_t& free);
	void insertDisk(VDisk* vd);
	DiskInfo getDiskInfo(VDisk* vd);
	std::string getDiskDBFile(); 
	std::string getOneKeyCopyDir(std::string sn);
	int getPartitionNumber(std::string devName);
	std::string getDiskTopDir(std::string devName, int partitionIdx);
	std::string getBackupDir(std::string topDir);
	std::string getUserDir(std::string username);
	int localDiskDeviceCount();

	Result handleRequest(int code, std::string& request);
    void response(int code, std::string request, Result result);
	
private:
	DiskManager();
	
	std::string 		mDBFilePath;
	SQLiteDatabase* 	mSQLiteDB;
    DiskTable*      	mDiskTable;
	pthread_mutex_t 	mMutex;
	DiskMngMsgHandler* 	mMsgHandler;
	std::vector<LocalDisk*>  mLocalDiskArray;
	std::vector<RemovableDisk*>  mRemovableDiskArray;
	std::vector<BackupDisk*>  mBackupDiskArray;
	std::vector<CopyDisk*>	mCopyDiskArray;			
};

void DiskMngInit();

} /* namespace IBox */

#endif // __DiskMng__H_
