#ifndef _NativeHandlerRunning_H_
#define _NativeHandlerRunning_H_

#include "NativeHandlerPublic.h"

#ifdef __cplusplus

namespace IBox {

class NativeHandlerRunning : public NativeHandlerPublic {
public:
    NativeHandlerRunning();
    ~NativeHandlerRunning();

    virtual NativeHandler::State state() { return NativeHandler::kRunning; }
    virtual void onActive();
    virtual void onUnactive();
    virtual bool handleMessage(Message *msg);

protected:

};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerRunning_H_
