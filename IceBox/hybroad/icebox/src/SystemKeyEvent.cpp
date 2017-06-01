#include "Assertions.h"
#include "SystemKeyEvent.h"
#include "MessageTypes.h"
#include "DiskDeviceManager.h"

#include <unistd.h>

#define REGIST_BOUND_MS 1000
#define POWER_BOUND_MS  3010
#define RESET_BOUND_MS  5010
#define UNDEF_BOUND_MS  7010

namespace IBox {

SystemKeyEvent::SystemKeyEvent()
	: Thread(), H(defNativeHandler())
{
}

SystemKeyEvent::~SystemKeyEvent()
{
}

void 
SystemKeyEvent::translate()
{
    int what = MessageType_Unknow;
    int arg1 = -1;
    int arg2 = -1;

    uint32_t kind = mEvent.eventkind;
    uint32_t value = mEvent.keyvalue;

    switch (kind) {
    case YX_EVENT_SYSTEM:
        what = MessageType_System;
        switch (value) {
        case HW_POWERDOWN:
            arg1 = MV_SYSTEM_POWER_DOWN;
            arg2 = 0;
            break;
        case HW_CHARGEFULL:
            arg1 = MV_SYSTEM_UPS_CHARGE;
            arg2 = 1;
            break;
        case HW_OVERCHARGE:
            arg1 = MV_SYSTEM_UPS_CHARGE;
            arg2 = 2;
        case HW_HD1_DOOR_OPEN:
            arg1 = MV_SYSTEM_HDISK0_DOOR;
            arg2 = 1;
            break;
        case HW_HD1_DOOR_CLOSE:
            arg1 = MV_SYSTEM_HDISK0_DOOR;
            arg2 = 0;
            break;
        case HW_HD2_DOOR_OPEN:
            arg1 = MV_SYSTEM_HDISK1_DOOR;
            arg2 = 1;
            break;
        case HW_HD2_DOOR_CLOSE:
            arg1 = MV_SYSTEM_HDISK1_DOOR;
            arg2 = 0;
            break;
        }
        break;
    default:
        ;
    }

    if (what != MessageType_Unknow)
        H.sendMessage(H.obtainMessage(what, arg1, arg2, 0));
}

void 
SystemKeyEvent::onHotplug()
{
    uint8_t device = mEvent.device; /* 1:usb,2:ata,3:sdcard */
    uint8_t action = mEvent.eventkind; /* in,out */
    uint8_t label = mEvent.status; /* ata1,usb3 */
    uint32_t value = mEvent.keyvalue;  /* sda,sdb,mmcblk1 */

    int arg1 = (action == YX_EVENT_HOTPLUG_ADD) ? DISK_PLUG_ADD : DISK_PLUG_REMOVE;
    int arg2 = 0;
    switch (device) {
    case 1: //USB
    case 2: //SATA
    case 3: //SD (mmc)
        arg2 = (int)(device << 16) | (int)(label << 8) | (int)value;
        Disk().doDiskChangeEvent(arg1, arg2);
        return;
    default:
        ;
    }
}

void 
SystemKeyEvent::onMountDisk()
{
    uint8_t device = mEvent.device; /* 1: usb 2:sdcard */
    uint8_t action = mEvent.eventkind; /* mount, unmount */
    uint8_t part = mEvent.status; /* sda1 sda2 */
    uint32_t value = mEvent.keyvalue;

    int arg1 = (action == YX_EVENT_MOUNT) ? DISK_MOUNT : DISK_UNMOUNT;
    int arg2 = 0;
    //TODO sdk have some problem.  2 is sdcard ?
    switch (device) {
    case 1: //USB and SATA
        arg2 = (int)(1 << 16) | (int)(part << 8) | (int)value;
        Disk().doDiskChangeEvent(arg1, arg2);
        return;
    case 2: //SD (mmc)
        arg2 = (int)(3 << 16) | (int)(part << 8) | (int)value;
        Disk().doDiskChangeEvent(arg1, arg2);
        return;
    default:
        ;
    }
}

void
SystemKeyEvent::onPressKey()
{
    int what = MessageType_Unknow;
    uint8_t device = mEvent.device;
    uint8_t action = mEvent.eventkind;
    uint32_t value = mEvent.keyvalue;

    int arg1 = -1;
    int arg2 = -1;

    switch (value) {
    case YX_IR_LEFT:
        arg1 = MV_PRESSKEY_LEFT;
        break;
    case YX_IR_UP:
        arg1 = MV_PRESSKEY_UP;
        break;
    case YX_IR_RIGHT: 
        arg1 = MV_PRESSKEY_RIGHT;
        break;
    case YX_IR_DOWN:	
        arg1 = MV_PRESSKEY_DOWN;
        break;
    case YX_IR_ENTER: 
        arg1 = MV_PRESSKEY_ENTER;
        break;
    case YX_FP_POWER:
        arg1 = MV_PRESSKEY_POWER;
        break;
    }

    switch (action) {
    case YX_EVENT_KEYUP:
        what = MessageType_KeyUp;
        if (YX_FP_FUNCTION == mPressKey) {
            arg2 = SysTime::GetMSecs() - mClockTime;
            H.removeMessages(MessageType_KeyDown);
            if (arg2 > UNDEF_BOUND_MS)
                arg1 = MV_PRESSKEY_UNDEF;
            else if (arg2 > RESET_BOUND_MS)
                arg1 = MV_PRESSKEY_RESET;
            else if (arg2 > POWER_BOUND_MS)
                arg1 = MV_PRESSKEY_POWER;
            else if (arg2 > REGIST_BOUND_MS)
                arg1 = MV_PRESSKEY_REGISTUSER;
            else
                arg1 = MV_PRESSKEY_SHORTFP;
        }
        break;
    case YX_EVENT_KEYDOWN:
         what = MessageType_KeyDown;
         mPressKey = value;
         if (YX_FP_FUNCTION == mPressKey) {
             mClockTime = SysTime::GetMSecs();
             arg1 = MV_PRESSKEY_SHORTFP;
             arg2 = 0;
             H.sendMessageDelayed(H.obtainMessage(what, MV_PRESSKEY_REGISTUSER, arg2, 0), REGIST_BOUND_MS);
             H.sendMessageDelayed(H.obtainMessage(what, MV_PRESSKEY_POWER, arg2, 0), POWER_BOUND_MS);
             H.sendMessageDelayed(H.obtainMessage(what, MV_PRESSKEY_RESET, arg2, 0), RESET_BOUND_MS);
             H.sendMessageDelayed(H.obtainMessage(what, MV_PRESSKEY_UNDEF, arg2, 0), UNDEF_BOUND_MS);
         }
         break;
    case YX_EVENT_KEYPRESS:
         break;
    default:
        break;
    }
    if (what != MessageType_Unknow)
        H.sendMessage(H.obtainMessage(what, arg1, arg2, 0));
}

void 
SystemKeyEvent::run() 
{
    int ret = -1;
    while (1) {
        memset(&mEvent, 0, sizeof(InputEvent));
        ret = yhw_input_getEvent(&mEvent, 50);
        if (ret != 0)
            continue;

        SYSTEM_LOG("device:0x%x, key:0x%x/%d type:0x%x, stat:0x%x\n", mEvent.device, mEvent.keyvalue, mEvent.keyvalue, mEvent.eventkind, mEvent.status);
        switch (mEvent.eventkind) {
        case YX_EVENT_HOTPLUG_ADD:
        case YX_EVENT_HOTPLUG_REMOVE: 
            onHotplug();
            break;
        case YX_EVENT_MOUNT: //0x80
        case YX_EVENT_UNMOUNT:
            onMountDisk();
            break;
        case YX_EVENT_KEYUP:
        case YX_EVENT_KEYDOWN:
        case YX_EVENT_KEYPRESS:
            onPressKey();
            break;
        default:
            translate(); 
        } 
    }
    SYSTEM_LOG_ERROR("Never run here!\n");
}

void SystemKeyEventInit()
{
    static SystemKeyEvent *s = 0;
    if (!s) {
        s = new SystemKeyEvent();
        s->start();
    }
}

} /* namespace IBox */
