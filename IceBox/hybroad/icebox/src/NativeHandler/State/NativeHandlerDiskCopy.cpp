#include "NativeHandlerAssertions.h"
#include "NativeHandlerDiskCopy.h"
#include "FileExplorerThread.h"
#include "Message.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "StringData.h"
#include "JsonObject.h"
#include "DiskDeviceManager.h"
#include "Canvas.h"
#include "DiskCopyQueue.h"
#include "DiskCopyView.h"
#include "GraphicsLayout.h"
#include "ImageView.h"
#include "PromptMessage.h"

namespace IBox {

NativeHandlerDiskCopy::NativeHandlerDiskCopy() : mDiskCopyQueue(0), mDiskCopyView(0)
{
}

NativeHandlerDiskCopy::~NativeHandlerDiskCopy()
{
}

void
NativeHandlerDiskCopy::onActive()
{
    NATIVEHANDLER_LOG("\n");
    mDiskCopyQueue = Disk().getDiskCopyQueue();
    if (!mDiskCopyView) {
        Desktop().showDiskCopyLayer();
        mDiskCopyView = Desktop().getDiskCopyView();
    }
    H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_SCAN, 0, 0)); 
}

void
NativeHandlerDiskCopy::onUnactive()
{ 
    H.removeMessages(MessageType_Disk);
    Desktop().clearDiskCopyLayer();
    mDiskCopyView = 0;
    NATIVEHANDLER_LOG("\n");
}

bool
NativeHandlerDiskCopy::handleMessage(Message* msg)
{
    if (msg->what != MessageType_DesktopRefresh && msg->what != MessageType_Repaint)
        NATIVEHANDLER_LOG("what = 0x%x, arg1 = 0x%x, arg2 = 0x%x\n", msg->what, msg->arg1, msg->arg2);

    if(!mDiskCopyView)
        return false;
    std::string tempbuff("");
    char* diskname = 0; 
    switch (msg->what) {
    case MessageType_Disk:
        switch (msg->arg1) {
        case MV_DISK_REMOVE:
            if (msg->obj) {
                JsonObject jo(dynamic_cast<StringData*>(msg->obj)->getData());
                std::string diskname = jo.getString("Disk");
                if (!mDiskName.compare(diskname)) {
                    onDiskCopyScanPrompt(eDC_DiskPullOut);
                    H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_NEXT, 1, 0), SHOW_TIME_MSEC); 
                }
                mDiskCopyQueue->delUDisk(diskname);  
            }
            break;
        case MV_DISK_COPY_SCAN:
            if (eDC_OK == onDiskCopyScanUI()) {
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_WAITSCAN, 0, 0), 1000);
                return true;
            }
            onDiskCopyScanPrompt(eDC_QueueNull);
            H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_END, 0));
            break;
        case MV_DISK_COPY_WAITSCAN:
            if (mDiskCopyQueue->getScanStatus(mDiskName)) {
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_WAITATA, 0, 0), 1000);
                return true;
            }
            if (msg->arg2 < SCAN_TIMEOUT_CNT) {
                onDiskCopyScanPrompt(eDC_ScanProgress, msg->arg2);
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, msg->arg1, msg->arg2 + 1, 0), 1000);
                return true;
            }
            onDiskCopyScanPrompt(eDC_ScanTimeout);
            H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_END, 0), SHOW_TIME_MSEC);
            break;
        case MV_DISK_COPY_WAITATA:
            switch (onDiskCopyCheckSpace()) {
            case eDC_ATANotReady:
                if (msg->arg2 < MAX_TRY_COUNT) {
                    onDiskCopyScanPrompt(eDC_ScanProgress, msg->arg2);
                    H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, msg->arg1, msg->arg2 + 1, 0), 1000);
                    return true;
                }
                onDiskCopyScanPrompt(eDC_ATANotReady);
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_END, 0), SHOW_TIME_MSEC);
                break;
            case eDC_ATASpaceErr:
                onDiskCopyScanPrompt(eDC_ATASpaceErr);
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_NEXT, 0, 0), SHOW_TIME_MSEC); 
                break;
            case eDC_ATAEnoughSpace:
                onDiskCopyScanPrompt(eDC_ATAEnoughSpace);
                break;
            }
            break;
        case MV_DISK_COPY_PROGRESS:
            onDiskCopyScanPrompt(eDC_CopyProgress, msg->arg2);
            if (msg->arg2 >= 100)
                H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_NEXT, 0, 0), SHOW_TIME_MSEC); 
            break;
        case MV_DISK_COPY_NEXT:
            if (!msg->arg2)
                mDiskCopyQueue->delUDisk(mDiskName);
            H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_SCAN, 0));
            break;
        case MV_DISK_COPY_RECEIVE:
            if (msg->obj) {
                tempbuff = dynamic_cast<StringData*>(msg->obj)->getData();
                mDiskCopyQueue->setScanStatus(tempbuff, true);
            } else {
                //TODO
                mDiskCopyQueue->setScanStatus(mDiskName, true);
            }
            break;
        case MV_DISK_COPY_END:
            revertState();
            break;
        }
        break;
    case MessageType_KeyUp:
        if(msg->arg1 == MV_PRESSKEY_SHORTFP) {
            if (Prompt().getCurrentMsgID() == ePM_DiskCopyTimingDown) {
                Prompt().delMessage(ePM_DiskCopyTimingDown);
                tempbuff = Disk().getDiskDeviceInfoByDiskName(mDiskName, 0);
                FileExplorerThread::getInstance()->sendMessage(MV_DISK_COPY, 0, 0, tempbuff.c_str());
            }
        }
        break;
    case MessageType_Prompt:
        if (MV_PROMTP_TIMEOUT == msg->arg1) {
            switch (msg->arg2) {
            case ePM_DiskCopyTimingDown:
                H.sendMessage(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_NEXT, 0, 0)); 
                break;
            }
        }
        break;
    case MessageType_Database:
        if (MV_DATABASE_OPENERROR == msg->arg1) {
            onDiskCopyScanPrompt(eDC_ReadWrite);
            H.sendMessageDelayed(H.obtainMessage(MessageType_Disk, MV_DISK_COPY_NEXT, 0, 0), SHOW_TIME_MSEC); 
        }
        break;
    case MessageType_Upgrade:
        //delay send upgrade message
        return onDelay(msg);
    }
    return NativeHandlerPublic::handleMessage(msg);
}

int 
NativeHandlerDiskCopy::onDiskCopyScanUI()
{
    NATIVEHANDLER_LOG("\n");
    mDiskName = mDiskCopyQueue->getFrontDiskName();
    if (mDiskName.empty())
        return eDC_ERR;

    std::string jsonstr("");
    jsonstr = Disk().getDiskDeviceInfoByDiskName(mDiskName, 0);
    if(isJsonString(jsonstr.c_str())) {
        JsonObject jo(jsonstr);
        mTotalSize = atoll(jo.getString("TotalSize").c_str()) / 1024.0 / 1024.0;
        mAvailSize = atoll(jo.getString("AvailSize").c_str()) / 1024.0 / 1024.0;
    }
    mDiskCopyView->promptUNameMessage(mDiskName, mTotalSize, mAvailSize).inval(0);
    return eDC_OK;
}

int 
NativeHandlerDiskCopy::onDiskCopyCheckSpace()
{
    NATIVEHANDLER_LOG("\n");
    std::string ata1("");
    std::string ata2("");
    std::string isReady("");
    int flag = 0;
    float s = mTotalSize - mAvailSize, s1 = 0.0, s2 = 0.0;

    ata1 = Disk().getDiskDeviceInfoByBusLabel(BUSLABEL_HDD0, 0);
    if(isJsonString(ata1.c_str())) {
        JsonObject jata1(ata1);
        isReady = jata1.getString("IsMountedAll");
        if (!isReady.compare("1")) {
            s1 = atoll(jata1.getString("AvailSize").c_str()) / 1024.0 / 1024.0;
            flag = 1;
        }
    } 
    ata2 = Disk().getDiskDeviceInfoByBusLabel(BUSLABEL_HDD1, 0);
    if(isJsonString(ata2.c_str())) {
        JsonObject jata2(ata2);
        isReady = jata2.getString("IsMountedAll");
        if (!isReady.compare("1")) {
            s2 = atoll(jata2.getString("AvailSize").c_str()) / 1024.0 / 1024.0;
            flag = 1;
        }
    }
    if (flag) {
        if(s1 > s || s2 > s)
            return eDC_ATAEnoughSpace;
        else
            return eDC_ATASpaceErr;
    }
    return eDC_ATANotReady;
}

int 
NativeHandlerDiskCopy::onDiskCopyScanPrompt(int id, int progress)
{
    NATIVEHANDLER_LOG("PromptID: %d Progress: %d\n", id, progress);
    switch (id) {
    case eDC_QueueNull:
        break;
    case eDC_ATAEnoughSpace:
        mDiskCopyView->promptScanMessage().inval(0);
        Prompt().addMessageX(ePM_DiskCopyTimingDown, 10);
        break;
    case eDC_ATASpaceErr:
        mDiskCopyView->promptSpaceNotEnoughMessage().inval(0);
        break;
    case eDC_DiskPullOut:
        Prompt().delMessage(ePM_DiskCopyTimingDown);
        mDiskCopyView->promptUPullOut().inval(0);
        break;
    case eDC_CopyProgress:
        mDiskCopyView->setCopyProgress(progress).inval(0);
        break;
    case eDC_ScanProgress:
        mDiskCopyView->setScanProgress(progress).inval(0);
        break;
    case eDC_ATANotReady:
    case eDC_ScanTimeout:
    case eDC_ERR:
    case eDC_ReadWrite:
    case eDC_UndefinedErr:
        mDiskCopyView->promptDiskScanError().inval(0);
        break;
    }
    return 0;
}

}
