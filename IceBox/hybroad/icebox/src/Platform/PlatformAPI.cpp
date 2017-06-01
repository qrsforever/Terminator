#include "PlatformAssertions.h"
#include "PlatformAPI.h"
#include "HY7252API.h"

extern "C" {
#include "ys_interface.h"
#include "build_info.h"
}

namespace IBox {

PlatformAPI::PlatformAPI()
{
    printf("ICBOX buildinfo: Version:%s BuildUser:%s BuildTime:%s\n", getBuildVersionString(), getBuildUser(), getBuildTime());
    yhw_board_init();
}

PlatformAPI::~PlatformAPI()
{

}

const char*
PlatformAPI::getBuildUser()
{
    return BUILD_USER;
}

const char*
PlatformAPI::getBuildTime()
{
    return BUILD_TIME;
}

const char*
PlatformAPI::getBuildVersionString()
{
    return VERSION_STRING;
}

const int 
PlatformAPI::getBuildVersionNumber()
{
    return VERSION_NUMBER;
}

int 
PlatformAPI::setTZ(double tz)
{
    PLATFOMR_LOG("timezone = %.2f\n", tz);
    int ret = yu_setTimeZone(tz);
    tzset();
    return ret;
}

int 
PlatformAPI::getTZ(double *tz)
{
    double t = 0.0;
    int ret = (int)yu_getTimeZone(&t);
    if (tz)
        *tz = t;
    PLATFOMR_LOG("timezone = %.2f\n", t);
    return ret;
}

int 
PlatformAPI::reboot()
{
    return yhw_board_reboot();
}

int 
PlatformAPI::powerOff()
{
    return -1;
}

std::string 
PlatformAPI::getMAC(char tok) 
{
    char mac[6] = { 0 };
    char str[18] = { 0 };

    ys_cfg_get_mac_addr((void*)mac);
    if (!tok)
        sprintf(str, "%02X%02X%02X%02X%02X%02X", 
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    else 
        sprintf(str, "%02X%c%02X%c%02X%c%02X%c%02X%c%02X", mac[0], tok, 
            mac[1], tok, mac[2], tok, mac[3], tok, mac[4], tok, mac[5]);

    PLATFOMR_LOG("platform mac = %s\n", str);
    return str;
}

std::string 
PlatformAPI::getSerial()
{
    char serial[64] = { 0 };
    ys_cfg_get_serial_code(serial);

    PLATFOMR_LOG("palform serial: %s\n", serial);
    return serial;
}

std::string 
PlatformAPI::getChipID()
{
    return "";
}

int 
PlatformAPI::systemCall(const char* cmd, int* ret)
{
    PLATFOMR_LOG("systemCall: %s\n", cmd);
    return yos_systemcall_runSystemCMD((char*)cmd, ret);
}

PlatformAPI& Platform()
{
    static PlatformAPI* s_platform = 0;
    if (s_platform)
        return *s_platform;

#if defined(PLATFORM_HY7252)
    s_platform = new HY7252API();
#else
    s_platform = new PlatformAPI(); 
#endif

    return *s_platform;
}

}
