#include "DiskAssertions.h"
#include "DiskCopyQueue.h"
#include "Mutex.h"
#include <stdlib.h>
#include <unistd.h>

namespace IBox {

DiskCopyQueue::DiskCopyQueue()
{
    mQueueMutex = new Mutex();
    mDiskNames.clear();
}

DiskCopyQueue::~DiskCopyQueue()
{
    delete (mQueueMutex);
}

void 
DiskCopyQueue::addUDisk(std::string diskname)
{
    DISK_LOG("diskname = %s\n", diskname.c_str());
    if (diskname.empty())
        return;
    std::map<std::string, bool>::iterator it = mDiskNames.find(diskname);
    if (it != mDiskNames.end())
        return;
    mQueueMutex->lock();
    mDiskNames.insert(std::make_pair(diskname, false));
    mQueueMutex->unlock();
    return;
}

void 
DiskCopyQueue::delUDisk(std::string diskname)
{
    DISK_LOG("diskname = %s\n", diskname.c_str());
    if (diskname.empty())
        return;
    std::map<std::string, bool>::iterator it = mDiskNames.find(diskname);
    if (it == mDiskNames.end())
        return;
    mQueueMutex->lock();
    mDiskNames.erase(diskname);
    mQueueMutex->unlock();
    return;
}

void 
DiskCopyQueue::setScanStatus(std::string diskname, bool flg)
{
    if (diskname.empty())
        return;
    std::map<std::string, bool>::iterator it = mDiskNames.find(diskname);
    if (it == mDiskNames.end())
        return;
    mQueueMutex->lock();
    it->second = flg;
    mQueueMutex->unlock();
    return;
}

bool 
DiskCopyQueue::getScanStatus(std::string diskname)
{
    bool flg = false;
    if (diskname.empty())
        return false;
    std::map<std::string, bool>::iterator it = mDiskNames.find(diskname);
    if (it == mDiskNames.end())
        return false;
    mQueueMutex->lock();
    flg = it->second;
    mQueueMutex->unlock();
    return flg;
}

std::string 
DiskCopyQueue::getFrontDiskName()
{
    std::map<std::string, bool>::iterator it = mDiskNames.begin();
    if (it != mDiskNames.end())
        return it->first;
    return "";
}

}
