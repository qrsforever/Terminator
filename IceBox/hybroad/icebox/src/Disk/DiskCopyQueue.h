#ifndef __DiskCopyQueue__H_
#define __DiskCopyQueue__H_

#include <map>
#include <string>

namespace IBox {
class Mutex;
class DiskCopyQueue
{
public:
    DiskCopyQueue();
    ~DiskCopyQueue();

    void addUDisk(std::string diskname);
    void delUDisk(std::string diskname);
    std::string getFrontDiskName();

    void setScanStatus(std::string diskname, bool flg);
    bool getScanStatus(std::string diskname);
private:
    std::map<std::string, bool> mDiskNames;
    Mutex* mQueueMutex;
};
}

#endif
