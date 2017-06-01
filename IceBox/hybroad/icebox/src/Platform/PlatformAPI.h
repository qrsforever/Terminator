#ifndef __PLATFORM_API__H
#define __PLATFORM_API__H

#include "libzebra.h"
#include <string>

namespace IBox {

class PlatformAPI {
public:
    PlatformAPI();
    virtual ~PlatformAPI();

    const char* getBuildUser();
    const char* getBuildTime();
    const char* getBuildVersionString();
    const int getBuildVersionNumber();
    int setTZ(double tz);
    int getTZ(double *tz);

    virtual int reboot();
    virtual int powerOff();
    virtual std::string getMAC(char tok = 0);
    virtual std::string getSerial();
    virtual std::string getChipID();

    virtual int setHDPower(int index, bool enable) { return -1; }
    virtual int setUSBPower(int index, bool enable) { return -1; }
    virtual int setFanPower(bool enable) { return -1; }
    virtual int getFanSpeed() { return 0; }
    virtual int setFanSpeed(int speed) { return -1; }
    virtual int startFan() { return -1; }
    virtual int stopFan() { return -1; }

    virtual int getChargeStatus() { return -1; }
    virtual int getDiskDoorStatus(int) { return -1; }

    int systemCall(const char* cmd, int* ret = 0);
private:
};

PlatformAPI& Platform();

}

#endif


#if 0

第三部分增加两个函数：
yresult yhw_powerControl_getChargeStatus(int *status)； //status 为1 表示正在充电，为0表示已充满
yresult yhw_powerControl_getDiskDoorStatus(int index, int *status); //index为硬盘索引，status 为1表示硬盘舱门开启，为0表示关闭

第三部分函数及声明已上传到sdk的svn上

zhangting
 
1. 风扇接口：
yresult yhw_fan_init(void);
yresult yhw_fan_unInit(void);
yresult yhw_fan_setPowerEnabled(int bEnabled);
yresult yhw_fan_setSpeed(int speed);
yresult yhw_fan_getSpeed(int *speed);
yresult yhw_fan_start(void);
yresult yhw_fan_stop(void);

其中yhw_fan_init()/yhw_fan_unInit()已经在yhw_board_init()/yhw_board_unInit()里面调用了，不用再调用。
判断损坏只能通过yhw_fan_getSpeed()来判断，如果和你设置的速度偏太大，就可以判断为坏了。注意设置的速度和得到速度不是一对一的，有一定偏差，还有设置速度后，立即调用函数得到的速度不会是设置的速度，风扇转速会逐步上升和下降。

2.  硬盘/usb/sdcard的损坏，如果真是本身设备坏了，sdk程序也是查不到的，调用yhw_check_xxxx相关自检函数可以检测相关项目自检状态

3.  
//设置power down 时的callback
yresult yhw_powerControl_setPowerDownCallback(YX_COMMON_FUNCTION pCallback, void *context, int param);
// 设置电容过冲时的callback ,  这个应该可以作为电容损坏的判断
yresult yhw_powerControl_setOverChargeCallback(YX_COMMON_FUNCTION pCallback, void *context, int param);
//设置充电指示时的callback, 在充满时会回调此函数
yresult yhw_powerControl_setChargeIndicatorCallback(YX_COMMON_FUNCTION pCallback, void *context, int param);
//设置硬盘舱门状态转换的callback, callback的第一个参数为硬盘index, 表示硬盘1和硬盘2，第二个参数为舱门状态，1 为开启，2为关闭
yresult yhw_powerControl_setHDDoorCheckCallback(YX_COMMON_FUNCTION pCallback);

相应的system event:
	HW_POWERDOWN,
	HW_OVERCHARGE,
	HW_CHARGEFULL,
	
	HW_HD1_DOOR_OPEN,
	HW_HD2_DOOR_OPEN,
	HW_HD1_DOOR_CLOSE,
	HW_HD2_DOOR_CLOSE,

如果上层应用调用callback函数设置了callback, sdk在检测到相应状态时会调用callback函数，如果没有设置callback函数则上报相应system event事件

第三部分的函数和事件因硬件还有问题，没有测试，因此还没有上传。

/*
yresult yhw_powerControl_init(void);
yresult yhw_powerControl_unInit(void);
yresult yhw_powerControl_setHDEnabled(int index, int bEnabled);
yresult yhw_powerControl_setUSBEnabled(int index, int bEnabled);
yresult yhw_powerControl_setBTEnabled(int bEnabled);
yresult yhw_powerControl_setWifiEnabled(int bEnabled);
yresult yhw_powerControl_setPowerDown(void);
yresult yhw_powerControl_setPowerReset(void);
*/


#endif
