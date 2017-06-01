#ifndef __BaseSetting_H_
#define __BaseSetting_H_

namespace IBox {

class BaseSetting {
public:
    enum {
        eBS_BASE = 0,
        eBS_IPv4 = 1,
        eBS_IPv6 = 2,
        eBS_DHCP = 3,
    };
    virtual int type() { }; 
    ~BaseSetting() { };
protected:
    BaseSetting() { }
};

}

#endif
