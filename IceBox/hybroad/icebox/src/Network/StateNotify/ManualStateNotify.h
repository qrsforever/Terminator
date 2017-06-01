#ifndef __ManualSTATENOTIFY__H_
#define __ManualSTATENOTIFY__H_

#include "StateNotify.h"

#ifdef __cplusplus

namespace IBox {
    
class ManualStateNotify : public StateNotify {
public:
    ManualStateNotify(NetworkInterface* iface);
    ~ManualStateNotify();
    virtual int notify(int state);
};

}
#endif

#endif
