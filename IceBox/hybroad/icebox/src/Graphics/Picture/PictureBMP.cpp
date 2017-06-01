#include "GraphicsAssertions.h"

#include "PictureBMP.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

namespace IBox {

#pragma pack(push)
#pragma pack(1)
struct bmp_fileheader {
    uint16_t    bfType;
    uint32_t    bfSize;
    uint16_t    bfReverved1;
    uint16_t    bfReverved2;
    uint32_t    bfOffBits;
};

struct bmp_infoheader {
    uint32_t   biSize;
    int32_t    biWidth;
    int32_t    biHeight;
    uint16_t   biPlanes;
    uint16_t   biBitCount;
    uint32_t   biCompression;
    uint32_t   biSizeImage;
    uint32_t   biXPelsPerMeter;
    uint32_t   biYpelsPerMeter;
    uint32_t   biClrUsed;
    uint32_t   biClrImportant;
};

typedef struct {
    uint8_t   rgbBlue;
    uint8_t   rgbGreen;
    uint8_t   rgbRed;
    uint8_t   rgbReserved;
} RGBQUAD;

typedef struct { 
    uint8_t  rgbBlue;
    uint8_t  rgbGreen;
    uint8_t  rgbRed;
} RGBTRIPLE;

typedef struct {   // rgb1555
    uint8_t  rgbBlue : 5;
    uint8_t  rgbGreen : 5;
    uint8_t  rgbRed : 5;
} RGB1555;

typedef struct {   // rgb565
    uint8_t  rgbBlue : 5;
    uint8_t  rgbGreen : 6;
    uint8_t  rgbRed : 5;
} RGB565;

#pragma pack(pop)

PictureBMP::PictureBMP(const char* filepath) : Picture(filepath)
{

}

PictureBMP::~PictureBMP()
{

}

int 
PictureBMP::load()
{
    reset();
    int ret = -1, i, j;
    bmp_fileheader bf;
    bmp_infoheader bi;
    uint8_t *bits = 0, r, g, b;
    int width, height, stride, linesize, padding;
    int *line = 0, *row = 0, *pixel = 0;
    RGBQUAD tcolor[256];

    FILE* fp = fopen(mFilePath.c_str(), "rb");
    if (!fp) {
        GRAPHICS_LOG_ERROR("open %s mFilePath.c_str() error!\n");
        return -1;
    }

    ret = fread(&bf, 1, sizeof(bf), fp);
    if (ret != sizeof(bf)) {
        GRAPHICS_LOG_ERROR("no bmpheader.\n");
        goto Err;
    }

    ret = fread(&bi, 1, sizeof(bi), fp);
    if (ret != sizeof(bi)) {
        GRAPHICS_LOG_ERROR("no biheader.\n");
        goto Err;
    }

    if (bi.biCompression != BI_RGB) {
        GRAPHICS_LOG_ERROR("no support.\n");
        goto Err;
    }

    width = bi.biWidth;
    height = bi.biHeight;
    stride = width * 4;

    GRAPHICS_LOG("%s info: w[%d] h[%d] bit[%d] compress[%d]\n", mFilePath.c_str(), width, height, bi.biBitCount, bi.biCompression);

    linesize = (width * bi.biBitCount + 7) / 8;
    padding = 4 - (linesize % 4);
    if (padding == 4)
        padding = 0;
    linesize += padding;

    bits = (uint8_t*) malloc (stride * height);

	fseek(fp, bf.bfOffBits, SEEK_SET);
    switch (bi.biBitCount) {
    case 1:
    case 4:
    case 8:
        {
            fseek(fp, sizeof(bmp_fileheader) + sizeof(bi), SEEK_SET);
            if (bi.biClrUsed > 0) {
                ret = fread(tcolor, sizeof(RGBQUAD), bi.biClrUsed, fp);
                if (ret != bi.biClrUsed) {
                    GRAPHICS_LOG_ERROR("ret=%d, != sizeof(RGBTRIPLE) * bi.biClrUsed=%d\n", ret, sizeof(RGBTRIPLE) * bi.biClrUsed);
                    goto Err;
                }
            } else {
                ret = fread(tcolor, sizeof(RGBQUAD), 1 << bi.biBitCount, fp);
                if (ret !=  1 << bi.biBitCount) {
                    GRAPHICS_LOG_ERROR("ret=%d,  1 << bi.biBitCount=%d\n", ret,  1 << bi.biBitCount);
                    goto Err;
                }
            }
            line = (int*)malloc(linesize);
            fseek(fp, bf.bfOffBits, SEEK_SET);
            for (i = height - 1; i >= 0; i--) {
                uint8_t *t, ci=0, tmp;
                t = (uint8_t*)line;
                ret = fread(line, linesize, 1, fp);
                if (ret != linesize) {
                    GRAPHICS_LOG_ERROR("ret=%d, != linesize=%d\n", ret, linesize);
                    goto Err;
                }
                row = (int*)(bits + i * stride);
                for (j = 0; j < width; j++) {
                    switch (bi.biBitCount) {
                    case 1:
                        if ((j>0) & (j % 8 == 0))
                            t++;
                        tmp = ((*t & 0x0F) << 4) | (*t >> 4);
                        ci = 0x01 & (tmp >> (j % 8));
                        break;
                    case 4:	
                        if (j % 2) {
                            ci = *t >> 4;
                            t++;
                        } else
                            ci = *t & 0x0F;  
                        break;
                    case 8: 
                        ci = *t;
                        t++; 
                        break;
                    }
                    b = tcolor[ci].rgbRed;
                    g = tcolor[ci].rgbGreen;
                    r = tcolor[ci].rgbBlue;
                    pixel = row + j;
                    *pixel = 0xFF << 24 | b << 16 | g << 8 | r;
                }
            }
            free(line);
        }
        break;
    case 16:
        {
            line = (int*)malloc(linesize);
            for (i = height - 1; i >= 0; i--) {
                RGB1555 *t;
                ret = fread(line, 1, linesize, fp);
                if (ret != linesize) {
                    GRAPHICS_LOG_ERROR("ret=%d, != linesize=%d\n", ret, linesize);
                    goto Err;
                }
                t = (RGB1555*)line;
                row = (int*)(bits + i * stride);
                for (j = 0; j < width; j++) {
                    r = t->rgbBlue << 3;
                    g = t->rgbGreen << 3;
                    b = t->rgbRed << 3;
                    pixel = row + j;
                    *pixel = 0xFF << 24 | b << 16 | g << 8 | r;
                    t++;
                }
            }
            free(line);
        }
        break;
    case 24: // 3 * 8 need padding 4 * 8
        {
            uint8_t* lbuf = (uint8_t*)malloc(linesize - padding);
            uint8_t* p = 0;
            for (i = height-1; i >= 0; i--) {
                char tmp[8];
                row = (int*)(bits + i * stride);
                ret = fread(lbuf, 1, linesize - padding, fp);
                if (ret != linesize - padding) {
                    GRAPHICS_LOG_ERROR("ret=%d, != linesize - padding=%d\n", ret, linesize - padding);
                    goto Err;
                }
                p = lbuf;
                for (j = 0; j < width; j++) {
                    r = *p++;
                    g = *p++;
                    b = *p++;
                    pixel = row + j;
                    *pixel = 0xFF << 24 | b << 16 | g << 8 | r;
                }
                fread(&tmp, padding, 1, fp);
            }
            free(lbuf);
        }
        break;
    case 32:
        {
            uint8_t* lbuf = (uint8_t*)malloc(stride);
            uint8_t* p = 0, a;
            for (i = height - 1; i >= 0; i--) {
                ret = fread(lbuf, 1, stride, fp);
                if (ret != stride) {
                    GRAPHICS_LOG_ERROR("ret=%d, != stride=%d\n", ret, stride);
                    goto Err;
                }
                p = lbuf;
                row = (int*)(bits + i * stride);
                for (j = 0; j < width; j++) {
                    r = *p++;
                    g = *p++;
                    b = *p++;
                    a = *p++;
                    pixel = row + j;
                    *pixel = a << 24 | b << 16 | g << 8 | r;
                }
            }
            free(lbuf);
        }
        break;
    default:
        GRAPHICS_LOG_ERROR("bi.biBitCount=%d is unsupported.\n", bi.biBitCount);
        goto Err;
        break;
    }
    fclose(fp);

    mWidth = width;
    mHeight = height;
    mBitmap = bits;
    mBitDepth = 32;
    mColorSpace = C_ARGB;
    mStride = stride;
    mPicType = PIC_BMP;
    return 0;
Err:
    if (fp)
        fclose(fp);
    if (bits)
        free(bits);
    return -1;
}

}
