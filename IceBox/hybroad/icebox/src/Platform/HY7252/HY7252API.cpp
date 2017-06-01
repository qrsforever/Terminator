#include "PlatformAssertions.h"
#include "HY7252API.h"

extern "C" {
#include "ys_interface.h"
}

namespace IBox {

HY7252API::HY7252API()
{
}

HY7252API::~HY7252API()
{

}

int 
HY7252API::powerOff()
{
    PLATFOMR_LOG("yhw_powerControl_setPowerDown !\n");
    return yhw_powerControl_setPowerDown();
}

std::string 
HY7252API::getChipID()
{
    char uid[8] = { 0 };
    char str[17] = { 0 };
    
    ylib_CAsecurity_ReadChipID(0, (unsigned char*)uid, 0);
    sprintf(str, "%02X%02X%02X%02X%02X%02X%02X%02X", 
        uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7]);

    PLATFOMR_LOG("platform chipid: %s\n", str);
    return str;
}

int 
HY7252API::setHDPower(int index, bool enable)
{
    return yhw_powerControl_setHDEnabled(index, enable ? 1 : 0);
}

int 
HY7252API::setUSBPower(int index, bool enable)
{
    return yhw_powerControl_setUSBEnabled(index, enable ? 1 : 0);
}

int 
HY7252API::setFanPower(bool enable)
{
    return yhw_fan_setPowerEnabled(enable ? 1 : 0);
}

int 
HY7252API::getFanSpeed()
{
    int speed = 0;
    yhw_fan_getSpeed(&speed);
    return speed;
}

int 
HY7252API::setFanSpeed(int speed)
{
    return yhw_fan_setSpeed(speed); 
}

int 
HY7252API::startFan()
{
    return yhw_fan_start();
}

int 
HY7252API::stopFan()
{
    return yhw_fan_stop();
}

int 
HY7252API::getChargeStatus()
{
    int status = -1;
    yhw_powerControl_getChargeStatus(&status);
    return status;
}

int 
HY7252API::getDiskDoorStatus(int index)
{
    int status = -1;
    yhw_powerControl_getDiskDoorStatus(index, &status);
    return status;
}

}
