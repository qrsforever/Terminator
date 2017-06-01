#include "DiskAssertions.h"
#include "Partition.h"
#include "PlatformAPI.h"
#include "DiskDevice.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "JsonObject.h"

namespace IBox {

Partition::Partition(std::string name, DiskDevice* disk) 
    : mPartName(name)
    , mDisk(disk)
    , mIsMounted(false)
{
    init();
}

Partition::~Partition()
{
    DISK_LOG("Destruct Partition.\n");
    umount();
}

int 
Partition::init()
{
    int ret = -1;
    FILE *f;
    struct mntent *mnt;
    std::string fsname("/dev/");

    if (!(f = setmntent(MOUNTED, "r")))
        return -1;

    fsname.append(mPartName);

    char jsonstr[256] = { 0 };
    while ((mnt = getmntent(f))) {
        if (!strncmp(mnt->mnt_dir, "/mnt/usb", 8) && access(mnt->mnt_fsname, F_OK)) {
            std::string cmd("umount -f");
            cmd.append(" ");
            cmd.append(mnt->mnt_dir);
            for (int i = 1; i <= MAX_TRY_COUNT; ++i) {
                Platform().systemCall(cmd.c_str(), &ret);
                if (!ret)
                    break;
                usleep(TRY_USLEEP_US);
            }
            DISK_LOG("%s umount on %s is out of time!\n", mnt->mnt_fsname, mnt->mnt_dir);
            continue;
        }
        if (!fsname.compare(mnt->mnt_fsname)) {
            mMountDir = mnt->mnt_dir;
            mFsType = mnt->mnt_type;
            mIsMounted = true;
            break;
        }
    }
    endmntent(f);
    return 0;
}

int 
Partition::refresh()
{
    //TODO
    umount();
    return init();
}

std::string 
Partition::getFilesystemType()
{
    FILE *f;
    struct mntent *mnt;
    std::string fstype("");

    if (!(f = setmntent(MOUNTED, "r")))
        return "";

    while ((mnt = getmntent(f))) {
        if (!mMountDir.compare(mnt->mnt_dir)) {
            fstype = mnt->mnt_type;
            break;
        }
    }
    endmntent(f);

    return fstype;
}

int 
Partition::mount()
{
    if (isMounted())
        return 0;

    std::string mntdir = getAvailMountDir();

    if (mntdir.empty())
        return -1;

    int ret;

    for (int i = 1; i <= MAX_TRY_COUNT; ++i) {
        ret = -1;
        std::string cmd("mount");
        if ( i == 1)
            cmd.append(" -o utf8=1"); // if type is extx, cannot use the options.
        cmd.append(" /dev/");
        cmd.append(mPartName);
        cmd.append(" ");
        cmd.append(mntdir);

        Platform().systemCall(cmd.c_str(), &ret) ;
        DISK_LOG("run: %d %s result: %d\n", i, cmd.c_str(), ret);
        if (!ret) {
            // mIsMounted = true;
            mMountDir = mntdir;
            mFsType = getFilesystemType();
            break;
        }
        usleep(TRY_USLEEP_US);
    }
    return ret;
}

int 
Partition::umount()
{
    if (!isMounted())
        return 0;

    int ret = -1;
    std::string cmd("umount -f");
    cmd.append(" ");
    cmd.append(mMountDir);

    for (int i = 1; i <= MAX_TRY_COUNT; ++i) {
        yos_systemcall_runSystemCMD((char*)cmd.c_str(), &ret) ;
        if (!ret)
            break;
        DISK_LOG_ERROR("run: %s error try %d after %d(us)\n", cmd.c_str(), i, TRY_USLEEP_US);
        usleep(TRY_USLEEP_US);
    }
    DISK_LOG("run: %s result: %d\n", cmd.c_str(), ret);
    if (!ret) {
        // mIsMounted = false;
        mMountDir = "";
    }
    return ret;
}

int 
Partition::getSpace(unsigned long long* total, unsigned long long* avail)
{
    *total = 0;       
    *avail = 0;
    if (!isMounted())
        return -1;

    struct statfs s;                                              
    if (!statfs(mMountDir.c_str(), &s)) {         
        *total = s.f_blocks / 1024 * s.f_bsize;
        *avail = s.f_bavail / 1024 * s.f_bsize;
    } else 
        return -1;
    return 0; 
}

std::string
Partition::getAvailMountDir()
{
    FILE *f;
    struct mntent *mnt;

    if(!(f = setmntent(MOUNTED, "r")))
        return "";

    char* pstr = 0;
    int c;
    int marks[MAX_USB_PORTS + 1] = { 0 }; //marks[0] not used.

    while((mnt = getmntent(f))) {
        pstr = strstr(mnt->mnt_dir, "/mnt/usb");
        if (pstr) {
            pstr = pstr + strlen("/mnt/usb");
            if (isdigit(*pstr)) {
                c = atoi(pstr);
                if (c > MAX_USB_PORTS)
                    continue;
                marks[c] = 1;
            }
        }
    }
    endmntent(f);
    
    int i;
    char mntdir[16] = { 0 };
    for (i = 1; i <= MAX_USB_PORTS; ++i) {
        if (!marks[i])
            break;
    }
    if (i > MAX_USB_PORTS)
        return "";
    sprintf(mntdir, "/mnt/usb%d", i);
    return mntdir;
}

int
Partition::format(std::string fsname, int ft)
{
    int res = -1;
    // umount();
    // {
        Filesystem* fs = Filesystem::create(ft);
        if (fs) {
            res = fs->makefs(fsname.c_str());
            delete fs;
        }
    // }
    // mount();
    return res;
}

std::string 
Partition::toJsonString(int flag)
{
    unsigned long long total = 0, avail = 0;
    char totalStr[128] = { 0 };
    char availStr[128] = { 0 };

    getSpace(&total, &avail);
    snprintf(totalStr, 127, "%lluKB", total);
    snprintf(availStr, 127, "%lluKB", avail);

    JsonObject obj;
    if (flag) {
        obj.add("Disk", mDisk->diskname());
        obj.add("BusLabel", mDisk->buslabel());
        obj.add("BusType", mDisk->bustype());
        obj.add("SerialNumber", mDisk->getSerialNumber());
    }
    obj.add("PartName", mPartName);
    obj.add("MountStatus", mIsMounted ? "1" : "0");
    if (mIsMounted) 
        obj.add("MountDir", mMountDir);
    obj.add("PartType", mFsType);
    obj.add("PartTotalSize", totalStr);
    obj.add("PartAvailSize", availStr);
    return obj.toString();
}

}
