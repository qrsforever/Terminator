#ifndef __PictureJPG__H_
#define __PictureJPG__H_ 

#include "Picture.h"

namespace IBox {

class PictureJPG : public Picture {
public:
    PictureJPG(const char* file);
    ~PictureJPG();
    virtual int load();
};

}

#endif
