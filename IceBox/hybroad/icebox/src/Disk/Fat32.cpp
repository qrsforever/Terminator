#include "Fat32.h"
#include "PlatformAPI.h"

namespace IBox {

Fat32::Fat32()
{ 
}

int
Fat32::makefs(const char* fmdev)
{
    remove(mExecLog.c_str());

    int ret = -1;
    std::string mkcmd("mkdosfs -v -F 32");
    mkcmd.append(" ");
    mkcmd.append(fmdev);
    mkcmd.append(" > ");
    mkcmd.append(mExecLog);
    DISK_LOG("run : %s\n", mkcmd.c_str());
    Platform().systemCall(mkcmd.c_str(), &ret) ;
    return ret;
}

}
