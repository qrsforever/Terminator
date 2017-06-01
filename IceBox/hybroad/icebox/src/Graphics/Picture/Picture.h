#ifndef __Picture__H_
#define __Picture__H_ 

#include "Object.h"
#include <string>
#include <stdint.h>

#define BMP_ID 0x4D42
#define JPG_ID 0xFFD8FF
#define PNG_ID 0x474E5089
#define GIF_ID 0x38464947

namespace IBox {

class Picture : public Object {

protected:
    Picture(const char* filepath);
public:
    void reset();
    enum {
        C_RGB,
        C_ARGB,
    };
    enum {
        PIC_UNKNOW = 0,
        PIC_PNG = 1,
        PIC_JPG = 2,
        PIC_BMP = 3,
        PIC_GIF = 4,
    };
    static Picture* create(const char* filepath);
    virtual ~Picture();
    virtual int load() = 0;
    bool isValid();

    uint8_t* bitmap() { return mBitmap; }
    int width() { return mWidth; }
    int height() { return mHeight; }
    int stride() { return mStride; }
    int pictype() { return mPicType; }
    const char* file() { return mFilePath.c_str(); }

    void show();
protected:
    uint8_t* mBitmap;
    int mWidth;
    int mHeight;
    int mStride;
    int mBitDepth;
    int mPicType;
    int mColorSpace;
    std::string mFilePath;
};

}

#endif
