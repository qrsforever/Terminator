#ifndef _NativeHandlerDiskCopy_H_
#define _NativeHandlerDiskCopy_H_

#include "NativeHandlerPublic.h"
#include <string>
#include "MessageTypes.h"

#ifdef __cplusplus

namespace IBox {

#define MAX_TRY_COUNT 5
#define SHOW_TIME_MSEC 2000
#define SCAN_TIMEOUT_CNT (15 * 60)

class DiskCopyView;
class DiskCopyQueue;
class NativeHandlerDiskCopy : public NativeHandlerPublic {
public:
    NativeHandlerDiskCopy();
    ~NativeHandlerDiskCopy();

    virtual NativeHandler::State state() {return NativeHandler::kDiskCopy;}
    virtual void onActive();
    virtual void onUnactive();
    virtual bool handleMessage(Message* msg);

protected:
    DiskCopyView* mDiskCopyView;    
    DiskCopyQueue* mDiskCopyQueue;

private:
    enum {
        eDC_ERR = -1,
        eDC_OK,
        eDC_QueueNull,
        eDC_ATANotReady, 
        eDC_ATAEnoughSpace, 
        eDC_ATASpaceErr,
        eDC_ScanTimeout,
        eDC_DiskPullOut,
        eDC_CopyProgress,
        eDC_ScanProgress,
        eDC_ReadWrite,
        eDC_UndefinedErr,
    };
    int onDiskCopyScanUI();
    int onDiskCopyCheckSpace();
    int onDiskCopyScanPrompt(int id, int progress = 0);
    
    std::string mDiskName;
    float mAvailSize;
    float mTotalSize;

};

} // namespace IBox

#endif // __cplusplus

#endif // _NativeHandlerDiskCopy_H_
