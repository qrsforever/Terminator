#ifndef __XmppFormatDisk__H_
#define __XmppFormatDisk__H_

#include "Thread.h"

#include <string>

#define FORMATDISK_SUCCESS  0
#define FORMATDISK_FAIL    -1
#define FORMATDISK_FINISH   1
#define FORMATDISK_PRESTART 2
#define FORMATDISK_START    3
#define FORMATDISK_PROGRESS 4

namespace IBox {

class MessageHandler;
class XmppFormatDisk {
public:
    friend class _Task;
    XmppFormatDisk(std::string jid, std::string part, std::string type, MessageHandler* handler);
    ~XmppFormatDisk();

    std::string& getAppJid() { return mAppJid; }
    std::string& getPartName() { return mPartition; }

    int getProgress();
    std::string getMountDir();
    unsigned long long getSize();

    bool isBusy() { return (mTaskState == _Task::_eRunning || mTaskState == _Task::_eReady); }
    int start(int force = 0);

    class _Task : public Thread {
    public :
        enum {
            _eReady = 0,
            _eRunning = 1,
            _eFinish = 2,
        };
        _Task(XmppFormatDisk& x) : _XmppFD(x) { }
        ~_Task() { };
        virtual void run();
    private:
        XmppFormatDisk& _XmppFD;
    };

    int mErrorCode;

private:
    int _ThreadFormat();
    int _ThreadError(int err);

private:
    int mTaskState;
    int mProgress;
    std::string mAppJid;
    std::string mPartition;
    std::string mType;
    MessageHandler* mHandler;
};

}

#endif
