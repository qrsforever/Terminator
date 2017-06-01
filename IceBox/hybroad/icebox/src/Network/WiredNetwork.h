#ifndef __WiredNetwork__H_
#define __WiredNetwork__H_

#include "NetworkCard.h"

#ifdef __cplusplus

namespace IBox {

class WiredNetwork : public NetworkCard {
public:
    WiredNetwork();
    ~WiredNetwork();
    
    int type() { return eCT_ETHERNET; }

    virtual int linkStatus(const char* ifname);
    virtual int flagChange(int flag);
};

}
#endif

#endif
