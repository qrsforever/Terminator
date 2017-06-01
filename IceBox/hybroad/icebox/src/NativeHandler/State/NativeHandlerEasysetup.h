#ifndef _NativeHandlerEasysetup_H_
#define _NativeHandlerEasysetup_H_

#include "NativeHandlerPublic.h"
#include <string>

#ifdef __cplusplus

namespace IBox {

class EasysetupView;
class NativeHandlerEasysetup : public NativeHandlerPublic {
public:
    NativeHandlerEasysetup();
    ~NativeHandlerEasysetup();

    virtual NativeHandler::State state() { return NativeHandler::kEasysetup; }
    virtual void onActive();
    virtual void onUnactive();
    virtual bool handleMessage(Message *msg);

    virtual bool doNetworkMessage(Message* msg);

    enum { 
        eCS_Unknow = 0,
        eCS_Success, 
        eCS_Fail, 
        eCS_Interrupt,
        eCS_Querying,
    };

protected:
    EasysetupView* mEasysetupView;
    std::string mIDCode;
    int mCurrentStatus;
};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerEasysetup_H_
