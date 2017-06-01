#include "Assertions.h"
#include "Picture.h"
#include "PictureBMP.h"
#include "PicturePNG.h"
#include "PictureJPG.h"
#include "PictureGIF.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace IBox {

Picture::Picture(const char* file) 
    : mWidth(0), mHeight(0), mStride(4), mBitDepth(0), mBitmap(0)
    , mFilePath(file), mPicType(PIC_UNKNOW), mColorSpace(C_RGB)
{
}

Picture::~Picture()
{
    if (mBitmap)
        free(mBitmap);
    mBitmap = 0;
}

void
Picture::reset()
{
    mWidth = 0;
    mHeight = 0;
    mStride = 4;
    mBitDepth = 0;
    if (mBitmap)
        free(mBitmap);
    mBitmap = 0;
    mPicType = PIC_UNKNOW;
}

bool
Picture::isValid() 
{ 
    return (mPicType != PIC_UNKNOW && mWidth > 0 && mHeight > 0);
}

Picture*
Picture::create(const char* filepath)
{
    FILE *fp = fopen(filepath,"rb");
    if (!fp) {
        RUNNING_LOG_ERROR("open %s error\n", filepath);
        return 0;
    }
    int header = 0;
    fread(&header, 4, 1, fp);
    fclose(fp);

    if ((header & 0xFFFFFF) == JPG_ID)
        return new PictureJPG(filepath);    
    
    if ((header & 0xFFFF) == BMP_ID)
        return new PictureBMP(filepath);

    if (header == PNG_ID)
        return new PicturePNG(filepath);

    if (header == GIF_ID)
        return new PictureGIF(filepath);

    return 0;
}

void 
Picture::show()
{
    RUNNING_LOG("file:%s w:%d h:%d bd:%d cp:%d type: %d \n", 
        mFilePath.c_str(), mWidth, mHeight, mBitDepth, mColorSpace, mPicType);
}

}
