#include "DiskAssertions.h"
#include "DiskDeviceManager.h"
#include "DiskDevice.h"
#include "MessageTypes.h"
#include "PlatformAPI.h"
#include "NativeHandler.h"
#include "Mutex.h"
#include "StringData.h"
#include "GraphicsLayout.h"
#include "DiskCopyQueue.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

//TODO must killall usbmount process.

namespace IBox {

DiskDeviceManager::DiskDeviceManager() : mTaskState(-1), mDiskCopyQueue(0)
{
    mQueMutex = new Mutex("DiskQueue"); 
    mDiskMutex = new Mutex("DiskMutex");
}

DiskDeviceManager::~DiskDeviceManager()
{
    //Never run here!
    mDiskMutex->lock();
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.begin();
    while (it != mByDiskNameDevs.end()) {
        delete it->second;
        ++it;
    }
    mByDiskNameDevs.clear();
    mByBusLabelDevs.clear();
    mDiskMutex->unlock();
    delete mQueMutex;
    delete mDiskMutex;
}

int 
DiskDeviceManager::init()
{
    DIR *dir = 0;                                                                                                     
    struct dirent *pdir = 0;

    NativeHandler& H = defNativeHandler();
    DiskDevice* disk = 0;
    StringData* data = 0;

    //DEBUG use.
    char cmd[32] = { 0 };
    int ret = 0;
    for (int i = 1; i < MAX_USB_PORTS + 1;  ++i) {
        sprintf(cmd, "umount /mnt/usb%d 2> /dev/null", i);
        Platform().systemCall(cmd, &ret);
    }

    dir = opendir("/sys/block");
    if (dir) {        
        while ((pdir = readdir(dir))) {
            disk = 0;
            if (!strncmp(pdir->d_name, "mmcblk", 6)) {
                if (isSDCard(pdir->d_name))
                    disk = addDiskDevice(pdir->d_name);
            } else {
                if (!strncmp(pdir->d_name, "sd", 2))
                    disk = addDiskDevice(pdir->d_name);
            }
            if (!disk)
                continue;
            std::string label = disk->buslabel();
            data = new StringData(getDiskDeviceInfoByDiskName(pdir->d_name, 1).c_str());
            if (data) {
                H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_ADD, 0, data));
                data->safeUnref();
            }
        }
        closedir(dir);
    }
    return 0;
}

bool
DiskDeviceManager::isSDCard(const char* diskname)
{
    FILE *fp = 0;
    char sysblock[256] = { 0 };
    char longlink[256] = { 0 };
    char line[256] = { 0 };
    char* pstr = 0;

    snprintf(sysblock, 255, "/sys/block/%s", diskname);
    int rsize = readlink(sysblock, longlink, 255);
    if (rsize < 0) {
        DISK_LOG_WARNING("readlink /sys/block/%s error!\n", diskname);
        return false;
    }
    pstr = strstr(longlink, "/block/"); 
    if (!pstr)
        return false;

    *pstr = 0;
    snprintf(sysblock, 255, "/sys/%s/type", longlink + 3);
    fp = fopen(sysblock, "r");
    if (!fp) {
        DISK_LOG_WARNING("can't open file %s\n", sysblock);
        return false;
    }
    fgets(line, 255, fp);
    fclose(fp);
    if (!strncmp(line, "SD", 2))
        return true;
    return false;
}

DiskDevice*
DiskDeviceManager::addDiskDevice(const char* diskname)
{
    DISK_LOG("add disk : %s\n", diskname);
    if (!diskname || !diskname[0])
        return 0;

    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.find(diskname);
    if (it != mByDiskNameDevs.end()) {
        it->second->refresh();
        return it->second;
    }

    mDiskMutex->lock();
    DiskDevice* disk = new DiskDevice(diskname);
    if (disk) {
        if (disk->buslabel().empty()) {
            delete disk;
            mDiskMutex->unlock();
            return 0;
        }
        mByDiskNameDevs.insert(std::make_pair(disk->diskname().c_str(), disk));
        mByBusLabelDevs.insert(std::make_pair(disk->buslabel().c_str(), disk));
        disk->mountPartitions();
        mDiskMutex->unlock();
        return disk;
    }
    mDiskMutex->unlock();
    return 0;
}

int
DiskDeviceManager::delDiskDevice(const char* diskname)
{
    DISK_LOG("del disk : %s\n", diskname);
    if (!diskname || !diskname[0])
        return 0;
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.find(diskname);
    if (it == mByDiskNameDevs.end())
        return 1;

    mDiskMutex->lock();
    DiskDevice* disk = it->second;
    if (disk) {
        disk->umountPartitions();
        mByDiskNameDevs.erase(disk->diskname());
        mByBusLabelDevs.erase(disk->buslabel());
        delete disk;
    }
    mDiskMutex->unlock();
    return 0;
}

DiskCopyQueue*
DiskDeviceManager::getDiskCopyQueue()
{
    if(mDiskCopyQueue == NULL)
        mDiskCopyQueue = new DiskCopyQueue();
    return mDiskCopyQueue;
}

DiskDevice*
DiskDeviceManager::getDiskDeviceByDiskName(std::string diskname)
{
    // DISK_LOG("get disk by diskname : %s\n", diskname);
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.find(diskname);
    if (it == mByDiskNameDevs.end())
        return 0;
    return it->second;
}

DiskDevice*
DiskDeviceManager::getDiskDeviceByBusLabel(std::string buslabel)
{
    // DISK_LOG("get disk by buslable : %s\n", buslabel);
    std::map<std::string, DiskDevice*>::iterator it = mByBusLabelDevs.find(buslabel);
    if (it == mByBusLabelDevs.end())
        return 0;
    return it->second;
}

Partition* 
DiskDeviceManager::getPartitionByPartName(std::string fsname)
{
    Partition* part = 0;

    mDiskMutex->lock();
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.begin();
    while (it != mByDiskNameDevs.end()) {
        part = it->second->getPartition(fsname);
        if (part)
            break;
        ++it;
    }
    mDiskMutex->unlock();
    return part;
}

Partition* 
DiskDeviceManager::getPartitionByMountDir(std::string mntdir)
{
    std::vector<std::string> fsnames;
    int i;
    Partition* part = 0;

    mDiskMutex->lock();
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.begin();
    while (it != mByDiskNameDevs.end()) {
        fsnames = it->second->getAllPartitionNames();
        for (i = 0; i < fsnames.size(); ++i) {
            part = it->second->getPartition(fsnames[i]);
            if (part) {
                if (!part->mntdir().compare(mntdir))
                    mDiskMutex->unlock();
                    return part;
            }
        }
        ++it;
    }
    mDiskMutex->unlock();
    return 0;
}

std::vector<std::string> 
DiskDeviceManager::getAllDiskNames()
{
    mDiskMutex->lock();
    std::vector<std::string> vnames;
    std::map<std::string, DiskDevice*>::iterator it = mByDiskNameDevs.begin();
    while (it != mByDiskNameDevs.end()) {
        vnames.push_back(it->second->diskname());
        ++it;
    }
    mDiskMutex->unlock();
    return vnames;
}

std::string 
DiskDeviceManager::getDiskDeviceInfoByDiskName(std::string diskname, int flag)
{
    DiskDevice* disk = getDiskDeviceByDiskName(diskname);
    if (!disk)
        return "";

    return disk->toJsonString(flag);
}

std::string 
DiskDeviceManager::getDiskDeviceInfoByBusLabel(std::string buslabel, int flag)
{
    DiskDevice* disk = getDiskDeviceByBusLabel(buslabel);
    if (!disk)
        return "";

    return disk->toJsonString(flag);
}

std::string 
DiskDeviceManager::getPartitionInfoByPartName(std::string partname, int flag)
{
    Partition* part = getPartitionByPartName(partname);
    if (!part)
        return "";

    return part->toJsonString(flag);
}

std::string 
DiskDeviceManager::getPartitionInfoByMountDir(std::string mntdir, int flag)
{
    Partition* part = getPartitionByMountDir(mntdir);
    if (!part)
        return "";

    return part->toJsonString(flag);
}

int
DiskDeviceManager::doDiskChangeEvent(int action, int value)
{
    DISK_LOG("doDiskChangeEvent [v:%d t:%06x]\n", action, value);
    mQueMutex->lock();
    mHotmsgs.push(_Hotmsg(action, value));
    mQueMutex->unlock();

    if (mTaskState == _Task::_eRunning)
        return 1;
    (new _Task(*this))->start();
    return 0;
}

void
DiskDeviceManager::_Task::run()
{
    char jsonstr[256] = { 0 };
    char diskname[16] = { 0 };
    char partname[16] = { 0 };

    NativeHandler& H = defNativeHandler();
    DiskDevice* disk = 0;
    StringData* data = 0;

    _DiskMgr.mTaskState = _eRunning;
    {
        while (!_DiskMgr.mHotmsgs.empty()) {
            _DiskMgr.mQueMutex->lock();
            _Hotmsg hotmsg = _DiskMgr.mHotmsgs.front();
            _DiskMgr.mQueMutex->unlock();

            int device = (hotmsg._value & 0x00FF0000) >> 16;
            int label = (hotmsg._value & 0x0000FF00) >> 8;
            int iname = (hotmsg._value & 0x000000FF);

            DISK_LOG("HotMessage: device[%d] label[%d] iname[%d]\n", device, label, iname);

            switch (device) {
            case DiskDevice::e_BUS_USB: //USB
            case DiskDevice::e_BUS_ATA: //SATA
                sprintf(diskname, "sd%c", (iname + 'a' - 1));
                sprintf(partname, "%s%d", diskname, label);
                break;
            case DiskDevice::e_BUS_SDC: //SD (mmc)
                sprintf(diskname, "mmcblk%c", iname + '0');
                sprintf(partname, "%sp%d", diskname, label);
                break;
            default:
                break;
            }

            switch (hotmsg._action) {
            case DISK_PLUG_ADD:
                disk = _DiskMgr.addDiskDevice(diskname);
                if (disk) {
                    std::string label = disk->buslabel();
                    data = new StringData(_DiskMgr.getDiskDeviceInfoByDiskName(diskname, 1).c_str());
                    if (data) {
                        H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_ADD, 0, data));
                        data->safeUnref();
                    }
                    DiskWidget(label).setRefreshFlag(REFRESH_FORCE_ONCE);
                }
                break;
            case DISK_PLUG_REMOVE:
                disk = _DiskMgr.getDiskDeviceByDiskName(diskname);
                if (disk) {
                    std::string label = disk->buslabel();
                    data = new StringData(_DiskMgr.getDiskDeviceInfoByDiskName(diskname, 1).c_str());
                    if (data) {
                        H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_REMOVE, 0, data));
                        data->safeUnref();
                    }
                    _DiskMgr.delDiskDevice(diskname);
                    DiskWidget(label).setRefreshFlag(REFRESH_FORCE_ONCE);
                }
                break;
            case DISK_MOUNT:
                disk = _DiskMgr.getDiskDeviceByDiskName(diskname);
                if (disk) {
                    Partition* part = disk->getPartition(partname);
                    if (!part)
                        break;
                    part->setMountFlg(true);
                    data = new StringData(_DiskMgr.getPartitionInfoByPartName(partname, 1).c_str());
                    if (data) {
                        H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_MOUNT, 0, data));
                        data->safeUnref();
                    }
                }
                break;
            case DISK_UNMOUNT:
                disk = _DiskMgr.getDiskDeviceByDiskName(diskname);
                if (disk) {
                    Partition* part = disk->getPartition(partname);
                    if (!part)
                        break;
                    part->setMountFlg(false);
                    data = new StringData(_DiskMgr.getPartitionInfoByPartName(partname, 1).c_str());
                    if (data) {
                        H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_UNMOUNT, 0, data));
                        data->safeUnref();
                    }
                }
                break;
            }

            _DiskMgr.mQueMutex->lock();
            _DiskMgr.mHotmsgs.pop();
            _DiskMgr.mQueMutex->unlock();
        }
    }
OUT:
    _DiskMgr.mTaskState = _eFinish;
    delete this;
}

DiskDeviceManager&
DiskDeviceManager::self()
{
    static DiskDeviceManager* s_ddm = 0;
    if (!s_ddm) {
        s_ddm = new DiskDeviceManager();
        s_ddm->init();
    }
    return *s_ddm;
}

}

//"{ \"Action\": \"FunctionCall\", \"FunctionType\": \"Format\", \"JID\": \"zpf@iz25b0xvgbmz\", \"Disk\": \"/dev/sda1\", \"FilesystemType\": \"NTFS\" }";
