#ifndef _NativeHandlerUpgrade_H_
#define _NativeHandlerUpgrade_H_

#include "NativeHandlerPublic.h"

#ifdef __cplusplus

namespace IBox {

class NativeHandlerUpgrade : public NativeHandlerPublic {
public:
    NativeHandlerUpgrade();
    ~NativeHandlerUpgrade();

    virtual NativeHandler::State state() { return NativeHandler::kUpgrade; }
    virtual void onActive();
    virtual void onUnactive();
    virtual bool handleMessage(Message *msg);

protected:

};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerUpgrade_H_
