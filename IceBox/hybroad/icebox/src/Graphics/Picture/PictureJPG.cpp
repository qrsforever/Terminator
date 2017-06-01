#include "Assertions.h"
#include "PictureJPG.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "jpeglib.h"
#include <setjmp.h>

namespace IBox {

typedef struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}* my_error_ptr;

static void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    (*cinfo->err->output_message) (cinfo);

    longjmp(myerr->setjmp_buffer, 1);
}

PictureJPG::PictureJPG(const char* filepath) : Picture(filepath)
{

}

PictureJPG::~PictureJPG()
{

}

int 
PictureJPG::load()
{
    FILE* fp = 0;
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    int bytesPerPix;

    fp = fopen(mFilePath.c_str(), "rb");
    if (!fp) {
        RUNNING_LOG_ERROR("open %s error!\n", mFilePath.c_str());
        return -1;
    }

    memset(&cinfo, 0, sizeof(jpeg_decompress_struct));
    memset(&jerr, 0, sizeof(jpeg_error_mgr));
    cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
        fclose (fp);
        return -1;
	}
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);
    jpeg_read_header(&cinfo, 1);

	cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    int width = cinfo.image_width;
    int height = cinfo.image_height;
    int stride = width * 4; //RGB24-->ARGB32

	bytesPerPix = cinfo.output_components;

    uint8_t *bits = (uint8_t *) malloc(stride * height);
    uint8_t *src = (uint8_t *) malloc(width * bytesPerPix);

    int *row = 0;
    int *pixel = 0;
    int i, j;
    for (i = 0; i < height; ++i) {
        jpeg_read_scanlines(&cinfo, &src, 1);
        row = (int*)(bits + stride * i);
        for (j = 0; j < width; ++j) {
            if (cinfo.jpeg_color_space == 1) {
                pixel = row + j;
                *pixel = 0xFF << 24 | src[j] << 16 | src[j] << 8 | src[j];
            } else {
                int bx = 3 * j;
                pixel = row + j;
                *pixel = 0xFF << 24 | src[bx + 0] << 16 | src[bx + 1] << 8 | src[bx + 2];	
            }
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);

    mWidth = width;
    mHeight = height;
    mBitmap = bits;
    mStride = stride;
    mColorSpace = C_ARGB;
    mPicType = PIC_JPG;
    return 0;
}

}
