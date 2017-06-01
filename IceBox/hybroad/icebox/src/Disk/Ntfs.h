#ifndef __NTFS__H_
#define __NTFS__H_ 

#include "Filesystem.h"

namespace IBox {

class Ntfs : public Filesystem {
public:
    Ntfs();
    ~Ntfs() { }
    FsType getType() { return e_FS_NTFS; }
    int makefs(const char* fmdev);
};
}

#endif 
