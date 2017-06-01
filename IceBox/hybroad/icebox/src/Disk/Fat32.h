#ifndef __FAT32__H_
#define __FAT32__H_ 

#include "Filesystem.h"

namespace IBox {

class Fat32 : public Filesystem {
public:
    Fat32();
    ~Fat32() { }
    FsType getType() { return e_FS_FAT32; }
    int makefs(const char* fmdev);
};
}

#endif 
