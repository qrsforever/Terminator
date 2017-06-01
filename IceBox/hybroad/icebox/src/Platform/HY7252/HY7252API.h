#ifndef __HY7252API__H_
#define __HY7252API__H_

#include "PlatformAPI.h"

namespace IBox {

class HY7252API : public PlatformAPI {
public:
    HY7252API();
    ~HY7252API();
    virtual int powerOff();
    virtual std::string getChipID();

    virtual int setHDPower(int index, bool enable);
    virtual int setUSBPower(int index, bool enable);
    virtual int setFanPower(bool enable);
    virtual int getFanSpeed();
    virtual int setFanSpeed(int speed);
    virtual int startFan();
    virtual int stopFan();

    virtual int getChargeStatus();
    virtual int getDiskDoorStatus(int);
};

}

#endif
