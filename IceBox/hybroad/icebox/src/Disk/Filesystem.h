#ifndef __Filesystem__H_
#define __Filesystem__H_ 

#include "DiskAssertions.h"
#include <mntent.h>       
#include <sys/mount.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <string>
#include <stdio.h>

extern "C" int yos_systemcall_runSystemCMD(char*, int*);

namespace IBox {

class Filesystem {
public:
    ~Filesystem() { }
    enum FsType {
        e_FS_FAT32,
        e_FS_EXT4,
        e_FS_NTFS,
    };

    static Filesystem* create(int fs_t);
    static int typeStr2Int(const char* strtype);
    virtual FsType getType() = 0;
    virtual int makefs(const char* fmdev) = 0;

protected:
    Filesystem();
    std::string mExecLog;
};

}

#endif
