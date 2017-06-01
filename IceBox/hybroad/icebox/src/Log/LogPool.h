#ifndef _LogPool_H_
#define _LogPool_H_

#include "DataSink.h"
#include "MessageHandler.h"

#ifdef __cplusplus

namespace IBox {

class LogFilter;

class LogPool : public DataSink, public MessageHandler {
public:
    LogPool();
    ~LogPool();

    virtual bool attachFilter(LogFilter*, int);
    virtual bool detachFilter(LogFilter*);

    virtual bool onDataArrive();
    virtual bool onError();
	virtual bool onEnd();

protected:
    enum MessageCode {
    	MC_DataArrive,
    	MC_End,
    	MC_Error
    };
    virtual void handleMessage(Message* msg);

    void receiveData();
    void receiveError();
    void receiveEnd();

private:
    LogFilter* m_filterHead;
};

} // namespace IBox

#endif // __cplusplus

#endif // _LogPool_H_
