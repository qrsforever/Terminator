#include "Filesystem.h"
#include "Fat32.h"
#include "Ntfs.h"
#include "Ext4.h"
#include <string.h>

namespace IBox {

Filesystem::Filesystem() : mExecLog("/var/run/mkdosfs.log")
{
}

Filesystem* 
Filesystem::create(int fs_t)
{
    switch (fs_t) {
    case e_FS_EXT4:
        return new Ext4();
    case e_FS_NTFS:
        return new Ntfs();
    case e_FS_FAT32:
        return new Fat32();
    default:
        return 0;
    }
    return 0;
}

int
Filesystem::typeStr2Int(const char* fstype)
{
    if (!strncasecmp(fstype, "FAT32", 5))
        return Filesystem::e_FS_FAT32;
    else if (!strncasecmp(fstype, "NTFS", 4))
        return Filesystem::e_FS_NTFS;
    else if (!strncasecmp(fstype, "EXT4", 4))
        return Filesystem::e_FS_EXT4;
    //TODO
    return Filesystem::e_FS_EXT4;
}

}
