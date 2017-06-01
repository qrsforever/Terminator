#ifndef __STATENOTIFY__H_
#define __STATENOTIFY__H_

#ifdef __cplusplus

namespace IBox {

class NetworkInterface;
class StateNotify {
public:
    virtual int notify(int state) = 0;
    virtual ~StateNotify() { }
protected:
    StateNotify(NetworkInterface* iface) : mIface(iface) { }
    NetworkInterface* mIface;
};

}

#endif

#endif
