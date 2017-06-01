#include "Assertions.h"
#include "PicturePNG.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "png.h"

namespace IBox {

PicturePNG::PicturePNG(const char* filepath) : Picture(filepath)
{

}

PicturePNG::~PicturePNG()
{

}

int 
PicturePNG::load()
{
    reset();
	FILE *fp = 0;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *rows;
	png_bytep p = 0, line = 0;
	int bit_depth, color_type;
    int width = 0, height = 0, stride = 0, size = 0;
    uint8_t* bits = 0;
    int i, j;
    uint8_t* q = 0;

    fp = fopen(mFilePath.c_str(), "rb");
	if (!fp) {
        RUNNING_LOG_ERROR("open %s error!\n", mFilePath.c_str());
        return -1;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) {
        RUNNING_LOG_ERROR("create png read error!\n");
        goto Err;
    }

    info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
        RUNNING_LOG_ERROR("create png info error!\n");
        png_destroy_read_struct(&png_ptr, 0, 0);
        goto Err;
    }

	if (setjmp(png_jmpbuf(png_ptr))) {
        RUNNING_LOG_ERROR("png jmp error!\n");
        goto Err;
    }

	png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 0);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR (png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth, &color_type, NULL, NULL, NULL);
    
    RUNNING_LOG("update before[%d %d %d %d]\n", width, height, bit_depth, color_type);

    if (16 == bit_depth)
        png_set_strip_16(png_ptr);

    if (bit_depth < 8)
        png_set_expand(png_ptr);

    if (PNG_COLOR_TYPE_PALETTE == color_type)
        png_set_expand(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))  
        png_set_expand(png_ptr);

    if (PNG_COLOR_TYPE_GRAY == color_type || PNG_COLOR_TYPE_GRAY_ALPHA == color_type)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    png_get_IHDR (png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth, &color_type, NULL, NULL, NULL);
    RUNNING_LOG("update after[%d %d %d %d]\n", width, height, bit_depth, color_type);

    stride = width * 4;
    size = stride * height;
	bits = (uint8_t*)malloc(size);
    if (!bits) {
        RUNNING_LOG_ERROR("malloc bits error!\n");
        goto Err;
    }
    line = (uint8_t*)malloc(stride);
    if (!line) {
        RUNNING_LOG_ERROR("malloc line error!\n");
        goto Err;
    }

    if (color_type == PNG_COLOR_TYPE_RGBA) {
        for (i = 0; i < height; i++) {
            png_read_row(png_ptr, line, NULL); 
            p = line;
            q = bits + i * stride;
            for (j = 0; j < width; j++) {
                q[0] = p[2]; //R
                q[1] = p[1]; //G
                q[2] = p[0]; //B
                q[3] = p[3];
                p += 4;
                q += 4;
            }
        }
    } else {
        for (i = 0; i < height; i++) {
            png_read_row(png_ptr, line, NULL); 
            p = line;
            q = bits + i * stride;
            for (j = 0; j < width; j++) {
                q[0] = p[2]; //R
                q[1] = p[1]; //G
                q[2] = p[0]; //B
                q[3] = 0xFF; 
                p += 3;
                q += 4;
            }
        }
    } 
    png_read_end(png_ptr, NULL);  
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
    free(line);

    mWidth = width;
    mHeight = height;
    mBitmap = bits;
    mStride = stride;
    mColorSpace = C_ARGB;
    mPicType = PIC_PNG;
    return 0;
Err:
    if (fp)
        fclose(fp);
    if (line)
        free(line);
    if (bits)
        free(bits);
    if (png_ptr && info_ptr)
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return -1;
}

}
