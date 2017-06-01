#ifndef __PictureBMP__H_
#define __PictureBMP__H_ 

#include "Picture.h"

#define BI_RGB 0

namespace IBox {

class PictureBMP : public Picture {
public:
    PictureBMP(const char* file);
    ~PictureBMP();
    virtual int load();
};

}

#endif
