#include "Ext4.h"
#include "PlatformAPI.h"

namespace IBox {

Ext4::Ext4() 
{ 
}

int
Ext4::makefs(const char* fmdev)
{
    remove(mExecLog.c_str());

    int ret = -1;
    std::string mkcmd("mkfs.ext4 -T largefile");
    mkcmd.append(" ");
    mkcmd.append(fmdev);
    mkcmd.append(" > ");
    mkcmd.append(mExecLog);
    DISK_LOG("run : %s\n", mkcmd.c_str());
    Platform().systemCall(mkcmd.c_str(), &ret) ;
    return ret;
}

}
