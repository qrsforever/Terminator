#include "Ntfs.h"
#include "PlatformAPI.h"

namespace IBox {

Ntfs::Ntfs()
{
} 

int
Ntfs::makefs(const char* fmdev)
{
    remove(mExecLog.c_str());

    int ret = -1;
    std::string mkcmd("mkntfs");
    mkcmd.append(" ");
    mkcmd.append(fmdev);
    mkcmd.append(" > ");
    mkcmd.append(mExecLog);
    DISK_LOG("run : %s\n", mkcmd.c_str());
    Platform().systemCall(mkcmd.c_str(), &ret);
    return ret;
}

}
