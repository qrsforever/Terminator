#ifndef _NativeHandlerPublic_H_
#define _NativeHandlerPublic_H_

#include "NativeHandler.h"

#ifdef __cplusplus

namespace IBox {

class NativeHandlerPublic : public NativeHandler::Callback {
public:

	NativeHandlerPublic();
	~NativeHandlerPublic();

	virtual bool handleMessage(Message *msg);
    virtual bool onDelay(Message*msg, int ms = 3000);

    virtual bool doSystemMessage(Message* msg);
    virtual bool doKeyDownMessage(Message* msg);
    virtual bool doKeyUpMessage(Message* msg);
    virtual bool doNetworkMessage(Message* msg);
    virtual bool doWeatherMessage(Message* msg);
    virtual bool doXmppMessage(Message* msg);
    virtual bool doDatabaseMessage(Message* msg);
    virtual bool doUpgradeMessage(Message* msg);
    virtual bool doPromptMessage(Message* msg);
    virtual bool doDiskMessage(Message* msg);

    virtual NativeHandler::State state() = 0;
    virtual void onActive() = 0;
    virtual void onUnactive() = 0;

protected:
    NativeHandler& H;

    int changeState(NativeHandler::State s) { return H.setState(s); }
    int revertState() { return H.setState(H.getOldState()); }

private:

};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerPublic_H_;
