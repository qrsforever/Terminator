#ifndef __Partition__H_
#define __Partition__H_

#include "Filesystem.h"

#define MAX_USB_PORTS 16
#define MAX_TRY_COUNT 5
#define TRY_USLEEP_US 500000

namespace IBox {

class DiskDevice;
class Partition {
public:
    enum PartionType {
        e_PT_PRIMARY,
        e_PT_EXTENDED,
        e_PT_LOGICAL,
    };
    Partition(std::string name, DiskDevice* disk);
    ~Partition();
    DiskDevice* getDiskDevice() { return mDisk; }

    std::string getFilesystemType();

    int init();
    int refresh();
    std::string& fsname() { return mPartName; }
    std::string& mntdir() { return mMountDir; }

    static int format(std::string devname, int ft = Filesystem::e_FS_EXT4);

    int mount();    // possible block
    int umount();   // possible block
    int getSpace(unsigned long long*, unsigned long long*);

    std::string getAvailMountDir();
    void setMountFlg(bool flg) { mIsMounted = flg; }
    bool isMounted() { return mIsMounted; }

    std::string toJsonString(int flag = 0);

private:
    DiskDevice* mDisk;
    std::string mPartName; // eg. sda1
    std::string mMountDir;  // eg. /mnt/usb1
    std::string mFsType;
    PartionType mPType;
    bool mIsMounted;
};

}

#endif
