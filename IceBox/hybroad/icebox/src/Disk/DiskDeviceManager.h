#ifndef __DiskDeviceManager__H_
#define __DiskDeviceManager__H_ 

#include "Thread.h"
#include "DiskDevice.h"
#include <queue>
#include <map>


#define DISK_PLUG_ADD    0
#define DISK_PLUG_REMOVE 1

#define DISK_MOUNT   10
#define DISK_UNMOUNT 11

#define DISK_FORMAT  20

#define BUSLABEL_HDD0 "ata1"
#define BUSLABEL_HDD1 "ata2"

#define BUSLABEL_USB0 "usb3"
#define BUSLABEL_USB1 "usb4"

#define BUSLABEL_SDCARD "mmc0"

namespace IBox {

class DiskDevice;
class Partition;
class Mutex;
class DiskCopyQueue;
class DiskDeviceManager {
public:
    friend class _Task;
    ~DiskDeviceManager();

    int init();

    static DiskDeviceManager& self();

    class _Task : public Thread {
    public :
        enum {
            _eRunning = 1,
            _eFinish = 2,
        };
        enum {
            _eTaskHotMessage = 0,
        };
        _Task(DiskDeviceManager& dm) : _DiskMgr(dm) { }
        ~_Task() { };
        virtual void run();
    private:
        std::string _Extra;
        DiskDeviceManager& _DiskMgr;
    };

    int doDiskChangeEvent(int, int);
    DiskCopyQueue* getDiskCopyQueue();
    //Warn: use the below function restrainedly. the disk device will be removed in season and out of season.
    DiskDevice* getDiskDeviceByDiskName(std::string diskname);
    DiskDevice* getDiskDeviceByBusLabel(std::string buslabel);
    Partition* getPartitionByPartName(std::string fsname);
    Partition* getPartitionByMountDir(std::string mntdir);

    //Good: you don't need care about whether the disk device removed or not.
    std::string getDiskDeviceInfoByDiskName(std::string diskname, int flag = 0);
    std::string getDiskDeviceInfoByBusLabel(std::string diskname, int flag = 0);
    std::string getPartitionInfoByPartName(std::string partname, int flag = 0);
    std::string getPartitionInfoByMountDir(std::string partname, int flag = 0);

    std::vector<std::string> getAllDiskNames();

    bool isSDCard(const char* diskname);

private:
    DiskDeviceManager();
    DiskDevice* addDiskDevice(const char* diskname);
    int delDiskDevice(const char* diskname);

private:
    struct _Hotmsg {
        _Hotmsg(int action, int value) : _action(action), _value(value) { }
        ~_Hotmsg() { }
        int _action;
        int _value;
    };
    int mTaskState;
    Mutex* mQueMutex; //for _Hotmsg
    Mutex* mDiskMutex;
    DiskCopyQueue* mDiskCopyQueue;
    std::queue<_Hotmsg> mHotmsgs;
    std::map<std::string, DiskDevice*> mByDiskNameDevs;
    std::map<std::string, DiskDevice*> mByBusLabelDevs;
};

inline DiskDeviceManager& Disk() { return DiskDeviceManager::self(); }

}

#endif
