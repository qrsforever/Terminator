#include "XmppAssertions.h"
#include "XmppFormatDisk.h"
#include "XmppShell.h"

#include "MessageHandler.h"
#include "DiskDeviceManager.h"
#include "DiskFormatView.h"

#include "JsonObject.h"

int TodoFileDB(int flg)
{
    return 1;
}

namespace IBox {

XmppFormatDisk::XmppFormatDisk(std::string jid, std::string part, std::string type, MessageHandler* handler) 
    : mHandler(handler), mTaskState(-1),  mErrorCode(-1)
    , mProgress(0), mAppJid(jid), mPartition(part), mType(type)
{
}

XmppFormatDisk::~XmppFormatDisk()
{

}

std::string 
XmppFormatDisk::getMountDir()
{
    Partition* part = Disk().getPartitionByPartName(mPartition); 
    if (!part)
        return "";
    return part->mntdir();
}

unsigned long long 
XmppFormatDisk::getSize()
{
    Partition* part = Disk().getPartitionByPartName(mPartition); 
    if (!part)
        return 0;
    unsigned long long t_total;
    unsigned long long t_avail;
    part->getSpace(&t_total, &t_avail);
    return t_total;
}

int
XmppFormatDisk::start(int force)
{
    if (mTaskState == _Task::_eRunning)
        return 1;

    if (!force) {
        if (!TodoFileDB(0))
            return _ThreadError(FORMAT_DISK_NOTFREE);
    }
    mProgress = 0;
    (new _Task(*this))->start();
    return 0;
}

int 
XmppFormatDisk::getProgress()
{
    //TODO 
    if (mProgress == 100)
        return 100;

    if (mProgress < 90)
        mProgress += 5;
    else 
        return 95;

    return mProgress;
}

int 
XmppFormatDisk::_ThreadError(int err)
{
    mErrorCode = err;
    if (mHandler) {
        if (err < 0)
            mHandler->sendMessage(mHandler->obtainMessage(XmppShell::e_HandleFormatDisk, FORMATDISK_FAIL, err, 0));
        else 
            mHandler->sendMessage(mHandler->obtainMessage(XmppShell::e_HandleFormatDisk, FORMATDISK_SUCCESS, err, 0));
    }
    return err;
}

int 
XmppFormatDisk::_ThreadFormat()
{
    mProgress = 5;
    int res = -1;

    Partition* part = Disk().getPartitionByPartName(mPartition); 
    if (!part)
        return _ThreadError(FORMAT_PARTITION_NOTFOUND);

    res = part->umount();
    if (0 != res)
        return _ThreadError(FORMAT_UNMOUNT_ERR);

    res = Partition::format(mPartition.c_str(), Filesystem::typeStr2Int(mType.c_str()));
    if (0 != res)
        return _ThreadError(FORMAT_EXECUTE_ERR);

    res = part->mount();
    if (0 != res)
        return _ThreadError(FORMAT_MOUNT_ERR);

    mProgress = 100;
    return _ThreadError(FORMAT_SUCCESS);
}

void
XmppFormatDisk::_Task::run()
{
    _XmppFD.mTaskState = _eRunning; 
    {
        _XmppFD._ThreadFormat();
    }
    _XmppFD.mTaskState = _eFinish;
    delete this;
}

}
