#ifndef __EXT4__H_
#define __EXT4__H_ 

#include "Filesystem.h"

namespace IBox {

class Ext4 : public Filesystem {
public:
    Ext4();
    ~Ext4() { }
    FsType getType() { return e_FS_EXT4; }
    int makefs(const char* fmdev);
};
}

#endif 
