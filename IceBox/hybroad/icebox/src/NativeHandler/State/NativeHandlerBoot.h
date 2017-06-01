#ifndef _NativeHandlerBoot_H_
#define _NativeHandlerBoot_H_

#include "NativeHandlerPublic.h"

#ifdef __cplusplus

namespace IBox {

class NativeHandlerBoot : public NativeHandlerPublic {
public:
    NativeHandlerBoot();
    ~NativeHandlerBoot();

    virtual NativeHandler::State state() { return NativeHandler::kBoot; }
    virtual void onActive();
    virtual void onUnactive();
    virtual bool handleMessage(Message *msg);

protected:

private:
    bool mBoxLoginOk;
    bool mNetConnectOk;
};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerBoot_H_
