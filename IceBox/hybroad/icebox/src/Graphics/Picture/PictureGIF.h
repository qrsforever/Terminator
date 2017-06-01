#ifndef __PictureGIF__H_
#define __PictureGIF__H_ 

#include "Picture.h"

namespace IBox {

class PictureGIF : public Picture {
public:
    PictureGIF(const char* file);
    ~PictureGIF();
    virtual int load();
};

}

#endif
