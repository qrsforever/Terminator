#ifndef __PicturePNG__H_
#define __PicturePNG__H_ 

#include "Picture.h"

namespace IBox {

#define LOAD_PNG_USE_CARIO

class PicturePNG : public Picture {
public:
    PicturePNG(const char* file);
    ~PicturePNG();
    virtual int load();
};

}

#endif
