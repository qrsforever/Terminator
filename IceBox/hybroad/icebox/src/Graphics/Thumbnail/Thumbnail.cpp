#include "GraphicsAssertions.h"
#include "Thumbnail.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "Buffer.h"

#include "jpeglib.h"
#include "png.h"
#include "gif_lib.h"

namespace IBox {

#pragma pack(push)
#pragma pack(2)
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
#pragma pack(pop)

void save_bmp(const char * path, const tools::Buffer& buffer, int width, int height, int depth)
{/*{{{*/
    bmp_fileheader  bf;
    bmp_infoheader  bi;

    memset(&bf, 0, sizeof(bf));
    memset(&bi, 0, sizeof(bi));

    bf.bfType       = 0x4d42;
    bf.bfSize       = sizeof(bmp_fileheader) + sizeof(bmp_infoheader) + (unsigned)buffer.length() + (depth == 8 ? 256 * 4 : 0);
    bf.bfOffBits    = sizeof(bmp_fileheader) + sizeof(bmp_infoheader);

    bi.biSize       = sizeof(bmp_infoheader);
    bi.biWidth      = width;
    bi.biHeight     = - height;
    bi.biPlanes     = 1;
    bi.biBitCount   = (uint16_t)depth;
    bi.biSizeImage  = width * height * depth / 8;

    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        GRAPHICS_LOG_ERROR("Open %s error!!!\n", path);
        return;
    }
    fwrite(&bf, 1, sizeof(bf), fp);
    fwrite(&bi, 1, sizeof(bi), fp);

    //灰度图像要添加调色板
    if (depth == 8) {
        unsigned char platte[256 * 4];
        for (int i = 0; i < 256 * 4; i += 4) {
            unsigned char j = i / 4;
            platte[i + 0] = j;
            platte[i + 1] = j;
            platte[i + 2] = j;
            platte[i + 3] = 0;
        }
        fwrite(platte, 1, 1024, fp);
    }
    fwrite(buffer.buffer(), 1, buffer.length(), fp);
    fclose(fp);
}/*}}}*/

struct my_error_mgr {                                             
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
}; 

typedef struct my_error_mgr * my_error_ptr;                                      
                                       
static void my_error_exit (j_common_ptr cinfo)
{                                      
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);    
}                                      

Thumbnail::Thumbnail()
    : interpolation(I_LINEAR), m_oper(OPER_NNN)
{
}

Thumbnail::~Thumbnail()
{
}

void Thumbnail::set_interpolation_type(INTERPOLATION interpolation)
{
    this->interpolation = interpolation;
}

int 
Thumbnail::loadImage(const char* inpath)
{
    bool result = false;
    FILE* fp = fopen(inpath, "rb");
    if (fp == NULL)
        return R_READ_FAILED;
    unsigned char    buf[16];
    int ret = fread(buf, 1, 4, fp);
    if (ret < 4)
        return R_CANNOT_RECOGNIZE;

    fseek(fp, 0, SEEK_SET);

	if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F' && buf[3] == '8') {
        // gif
        result = load_gif(inpath);
    } else if (buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && (buf[3] == 0xe0 || buf[3] == 0xe1)) {
        // jpeg
        result = load_jpeg(fp);
    } else if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G') {
        // png
        result = load_png(fp);
    } else if (buf[0] == 'B' && buf[1] == 'M') {
        // bmp
        result = load_bmp(fp);
    } else {
        fclose(fp);
        return R_CANNOT_RECOGNIZE;
    }
    fclose(fp);
    if(result == true) 
        return R_SUCCESS;
    else 
        return R_NOT_SUPPORT;
}

Thumbnail::RESULT Thumbnail::create(const char * inpath, const char * outpath, int quality, int maxSize)
{
    if (R_SUCCESS != loadImage(inpath))
        return R_CANNOT_RECOGNIZE;

    FILE* fpw = fopen(outpath, "wb");
    if (fpw == NULL) {
        fclose(fpw);
        return R_WRITE_FAILED;
    }

    if (m_width > m_height) {
        if (maxSize > m_width)
            maxSize = m_width;
        resample(maxSize, 0, fpw, quality);
    } else {
        if (maxSize > m_height)
            maxSize = m_height;
        resample(0, maxSize, fpw, quality);
    }

    fclose(fpw);
    return R_SUCCESS;
}

Thumbnail::RESULT Thumbnail::createBySize(const char * inpath, const char * outpath, int wsize, int hsize, int quality, int maxImgSize)
{
    //TODO maxImgSize not use.
    if (R_SUCCESS != loadImage(inpath))
        return R_CANNOT_RECOGNIZE;

    if (m_oper == OPER_CUTIMG) {
        if (!cut_squareimg())
            return R_CUT_FAILED;
    } else if (m_oper == OPER_FILLIMG) {
        if (!fill_squareimg(0x00 /*black*/))
            return R_FILL_FAILED;
    }

    FILE* fpw = fopen(outpath, "wb");
    if (fpw == NULL) {
        fclose(fpw);
        return R_WRITE_FAILED;
    }
    resample(wsize, hsize, fpw, quality);

    fclose(fpw);
    return R_SUCCESS;
}

Thumbnail::RESULT Thumbnail::createByScale(const char * inpath, const char * outpath, float scale, int quality, int maxImgSize)
{
    if (R_SUCCESS != loadImage(inpath))
        return R_CANNOT_RECOGNIZE;

    FILE* fpw = fopen(outpath, "wb");
    if (fpw == NULL) {
        fclose(fpw);
        return R_WRITE_FAILED;
    }
    resample(m_width * scale, m_height * scale, fpw, quality);
    fclose(fpw);
    return R_SUCCESS;
}
bool Thumbnail::fill_squareimg(int color)
{
    int i = 0, fill = 0, linebytes = 0;
    int num = m_depth / 8;
    const char* image = m_image_buffer.buffer();
    char* data = 0;
    if (m_width > m_height) {
        data = (char*)malloc(m_width * m_width * num);
        if(!data)
            return false;
        fill = (m_width - m_height) / 2;
        linebytes = m_width * num;
        memset(data, color, m_width * m_width * num);
        for(i = 0; i < m_height; i++)
            memcpy(data + (i + fill) * linebytes, image + i * linebytes, linebytes);
        m_height = m_width;
        m_image_buffer.assign(data, m_width * m_height * num);
        free(data);
    }if(m_width < m_height) {
        data = (char*)malloc(m_height * m_height * num);
        if(!data)
            return false;
        fill = (m_height - m_width) / 2;
        linebytes = m_width * num;
        memset(data, color, m_height * m_height  * num);
        for(i = 0; i < m_height; i++)
            memcpy(data + (fill * num + i * linebytes), image + i * linebytes, linebytes);
        m_width = m_height;
        m_image_buffer.assign(data, m_width * m_height * num);
        free(data);
    }
    return true;
}
bool Thumbnail::cut_squareimg()
{ 
    int i = 0, remov = 0, linebytes = 0;
    int num = m_depth / 8;
    const char* image = m_image_buffer.buffer();
    char* data = 0;
    if (m_width > m_height) {
        data = (char*)malloc(m_height * m_height * num);
        if (!data)
            return false;
        remov = (m_width - m_height) / 2;
        linebytes = m_height * num;
        for (i = 0; i < m_height; ++i)
            memcpy(data + i * linebytes, image + (i * m_width * num + remov * num), linebytes);
        m_width = m_height;
        m_image_buffer.assign(data, m_height * linebytes);
        free (data);
    } if (m_width < m_height) {
        data = (char*)malloc(m_width * m_width * num);
        if (!data)
            return false;
        remov = (m_height - m_width) / 2;
        linebytes = m_width * num;
        for (i = 0; i < m_width; ++i)
            memcpy(data + i * linebytes , image + (remov * m_width * num), linebytes);
        m_height = m_width;
        m_image_buffer.assign(data, m_width * linebytes);
        free (data);
    }
    return true;
}

bool Thumbnail::load_jpeg(FILE* fp)
{/*{{{*/
    if (fp == NULL) {
        GRAPHICS_LOG_ERROR("fp is NULL!!\n");
        return false;
    }

    struct jpeg_decompress_struct   cinfo;
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;                                                         
    if (setjmp(jerr.setjmp_buffer)) {
        GRAPHICS_LOG_ERROR("JPEG decompression Err.\n");
        jpeg_destroy_decompress(&cinfo);
        return false;
    }             
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);
    jpeg_read_header(&cinfo, TRUE);

    m_width = cinfo.image_width;
    m_height = cinfo.image_height;
    m_depth = cinfo.num_components * 8;

    GRAPHICS_LOG("m_depth = %d\n", m_depth);

    m_image_buffer.assign(m_width * m_height * m_depth / 8);

    jpeg_start_decompress(&cinfo);
    JSAMPROW row_pointer[1];

    while(cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = m_image_buffer + cinfo.output_scanline * m_width * m_depth / 8;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    if (m_depth == 8)
        color_space = C_GRAY;
    else 
        color_space = C_RGB;
    return true;
}/*}}}*/

bool Thumbnail::load_png(FILE* fp)
{/*{{{*/
    if (fp == NULL) {
        GRAPHICS_LOG_ERROR("fp is NULL!!\n");
        return false;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == NULL) {
        GRAPHICS_LOG_ERROR("png_ptr is NULL!!\n");
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        GRAPHICS_LOG_ERROR("info_ptr is NULL!!\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return false;
    }

    png_init_io(png_ptr, fp);

    // PNG_TRANSFORM_EXPAND:
    // 1.Expand paletted colors into true RGB triplets
    // 2.Expand grayscale images to full 8 bits from 1, 2, or 4 bits/pixel
    // 3.Expand paletted or RGB images with transparency to full alpha channels so the data will be available as RGBA quartets。
    // PNG_TRANSFORM_STRIP_ALPHA：Strip alpha bytes from the input data without combining withthe background
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);

    m_width = info_ptr->width;
    m_height = info_ptr->height;

    int color_type = info_ptr->color_type;
    m_depth = info_ptr->pixel_depth;
    int bit_depth = m_depth / 8;

    png_bytep* row_pointers = png_get_rows(png_ptr,info_ptr);

    GRAPHICS_LOG("m_depth=%d,,,color_type==%d\n", m_depth, color_type);
    int pos = 0;
    m_image_buffer.assign(m_width * m_height * bit_depth);
    switch(color_type) {/*
    case PNG_COLOR_TYPE_RGB_ALPHA:
        for(int i = 0; i < m_height; i++) {
            for(int j = 0; j < m_width * bit_depth; ) {
                m_image_buffer[pos++] = row_pointers[i][j++]; // alpha
                m_image_buffer[pos++] = row_pointers[i][j++]; // red
                m_image_buffer[pos++] = row_pointers[i][j++]; // green
                m_image_buffer[pos++] = row_pointers[i][j++]; // blue                
            }
        }
        break;*/
    case PNG_COLOR_TYPE_RGB:
        for(int i = 0; i < m_height; i++) {
            for(int j = 0; j < m_width * bit_depth; ) {
                m_image_buffer[pos++] = row_pointers[i][j++]; // red
                m_image_buffer[pos++] = row_pointers[i][j++]; // green
                m_image_buffer[pos++] = row_pointers[i][j++]; // blue
            }
        }
        break;
    default:
        GRAPHICS_LOG("run here.\n");
        break;
    }
    // free memory
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    color_space = C_RGB;
    return true;
}/*}}}*/

bool Thumbnail::load_bmp(FILE* fp)
{/*{{{*/
    bmp_fileheader  bf;
    bmp_infoheader  bi;
    int ret = fread(&bf, 1, sizeof(bf), fp);
    if (ret != sizeof(bf)) {
        GRAPHICS_LOG_ERROR("no bmpheader.\n");
        return false;
    }
    ret = fread(&bi, 1, sizeof(bi), fp);
    if (ret != sizeof(bi)) {
        GRAPHICS_LOG_ERROR("no biheader.\n");
        return false;
    }
    if (bi.biBitCount != 16 && bi.biBitCount != 24 && bi.biBitCount != 32) {
        GRAPHICS_LOG_ERROR("no support.\n");
        return false;
    }

    // 直接全按BI_BITFIELD解. 效率不够时再考率把BI_RGB形式的挪成代码.
    uint32_t rmask, gmask, bmask;
    if (bi.biCompression == 0) { // BI_RGB
        if (bi.biBitCount == 16) {
            rmask = 0x7c00;
            gmask = 0x3e0;
            bmask = 0x1f;
        } else if (bi.biBitCount == 24) {
            rmask = 0x00ff0000;
            gmask = 0x0000ff00;
            bmask = 0x000000ff;
        } else if (bi.biBitCount == 32) {
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
        } else {
            GRAPHICS_LOG_WARNING("\n");
        }
    } else if (bi.biCompression == 3) { // BI_BITFIELD
        int ret;
        fseek(fp, sizeof(bf) + sizeof(bi), SEEK_SET); 
        ret = fread(&rmask, 1, 4, fp);
        ret = fread(&gmask, 1, 4, fp);
        ret = fread(&bmask, 1, 4, fp);
        ret = ret;
    } else {
        GRAPHICS_LOG_ERROR("no support.\n");
        return false;
    } 


    fseek(fp, bf.bfOffBits, SEEK_SET);

    int pixel_depth = bi.biBitCount / 8;

    // BMP: 每行字节数是4的倍数.
    int src_bytes_per_line = bi.biWidth * pixel_depth;
    int dst_bytes_per_line = bi.biWidth * 3;

    while ((src_bytes_per_line % 4) !=0)
        src_bytes_per_line++;

    while ((dst_bytes_per_line % 4) !=0)
        dst_bytes_per_line++;

    m_image_buffer.assign(dst_bytes_per_line * abs(bi.biHeight));
    tools::Buffer   src;
    src.assign(src_bytes_per_line);

    // 直接做成24位RGB的.
    for (int y = 0; y < abs(bi.biHeight); y++) {
        int ret = fread(src, 1, src_bytes_per_line, fp);
        if (ret != src_bytes_per_line) {
            GRAPHICS_LOG_ERROR("read failed.\n");
            return false;
        }
        for (int x = 0; x < bi.biWidth; x++) {
            int offset_dst = dst_bytes_per_line * (bi.biHeight > 0 ? (bi.biHeight - y - 1) : y) + x * 3;

            uint32_t color = 0;
            memcpy(&color, src.buffer() + x * pixel_depth, pixel_depth);
            uint32_t rm = rmask;
            uint32_t gm = gmask;
            uint32_t bm = bmask;
            uint32_t r = (color & rm);
            uint32_t g = (color & gm);
            uint32_t b = (color & bm);
            while ((rm & 0x1) == 0) {
                rm >>= 1;
                r >>= 1;
            }
            while ((gm & 0x1) == 0) {
                gm >>= 1;
                g >>= 1;
            }
            while ((bm & 0x1) == 0) {
                bm >>= 1;
                b >>= 1;
            }
            r = r * 256 / (rm + 1);
            g = g * 256 / (gm + 1);
            b = b * 256 / (bm + 1);
            m_image_buffer[offset_dst + 0] = r & 0xff;
            m_image_buffer[offset_dst + 1] = g & 0xff;
            m_image_buffer[offset_dst + 2] = b & 0xff;
        }
    }

    m_width = bi.biWidth;
    m_height = abs(bi.biHeight);
    m_depth = 24;
    color_space = C_RGB;

    // save_bmp("b.bmp", m_image_buffer, m_width, m_height, m_depth);
    return true;
}/*}}}*/

// http://www.blogjava.net/georgehill/articles/6550.html
// http://www.yieyo.com/yieyohtml/IT/others/20141208_86.html
bool Thumbnail::load_gif(const char* path)
{/*{{{*/
    GifFileType * gf = DGifOpenFileName(path);
    if (gf == NULL) {
        GRAPHICS_LOG_ERROR("run here.\n");
        PrintGifError();
        return false;
    }

    m_width = gf->SWidth;
    m_height = gf->SHeight;
    m_depth = 24; // 直接做成24位的...
    int bit_depth = m_depth / 8;

    // 填充背景:
    GifRowType* screen = new GifRowType[gf->SHeight];
    if (screen == NULL) {
        GRAPHICS_LOG_ERROR("new error.\n");
        return false;
    }

    for (int j = 0; j < gf->SHeight; j++) {
        screen[j] = new GifByteType[gf->SWidth];
        if (screen[j] == NULL) {
            GRAPHICS_LOG_ERROR("new error.\n");
            return false;
        }
        for (int i = 0; i < gf->SWidth; i++) {
            // 虽然这里的 SBackGroundColor 是个 GifWord，但是实际上解码时它被赋了一个 GifByteType 类型的值。
            screen[j][i] = gf->SBackGroundColor;
        }
    }
    
    m_image_buffer.assign(gf->SWidth * gf->SHeight * bit_depth);
    
    // void* priv = (void*)(long*)gf->Private;
    // tools::dump(priv, 64 + 16);

    // GifFilePrivateType::File 偏移64字节.
    // GifFilePrivateType* p = (GifFilePrivateType*)gf->Private;
    //FILE* fp = *((FILE**)((char *)gf->Private + 64));
    //RUN_HERE() << "record start offset: 0x" << std::hex << ftell(fp);

    GifRecordType   rt;
    // 按块读.
    do {
        GRAPHICS_LOG("run here.\n");
        if (DGifGetRecordType(gf, &rt) == GIF_ERROR) {
            // meet some bad record...
            PrintGifError();
            continue;
        }

        GRAPHICS_LOG("record type = %d\n", rt);
        switch (rt) {
        case IMAGE_DESC_RECORD_TYPE:
            {
                if (DGifGetImageDesc(gf) == GIF_ERROR) {
                    PrintGifError();
                    for (int j = 0; j < gf->SHeight; j++) {
                        delete screen[j];
                    }
                    delete screen;
                    return false;
                }
                GifWord Top     = gf->Image.Top; /* Image Position relative to Screen. */
                GifWord Left    = gf->Image.Left;
                GifWord Width   = gf->Image.Width;
                GifWord Height  = gf->Image.Height;

                if (gf->Image.Left + gf->Image.Width > gf->SWidth || gf->Image.Top + gf->Image.Height > gf->SHeight) {
                    GRAPHICS_LOG_ERROR("Image is not confined to screen dimension, aborted.\n");
                    for (int j = 0; j < gf->SHeight; j++) {
                        delete screen[j];
                    }
                    delete screen;
                    return false;
                }

                if (gf->Image.Interlace) {
                    GRAPHICS_LOG("Interlaced.\n");
                    // Need to perform 4 passes on the images:
                    // Group 1：每隔8行组成一组，从第0行开始显示 /第1遍交插
                    // Group 2：每隔8行组成一组，从第4行开始显示 /第2遍交插
                    // Group 3：每隔4行组成一组，从第2行开始显示 /第3遍交插
                    // Group 4：每隔2行组成一组，从第1行开始显示 /第4遍交插
                    const int InterlacedJumps[] = { 8, 8, 4, 2 };
                    const int InterlacedOffset[] = { 0, 4, 2, 1 };
                    for (int i = 0; i < 4; i++) {
                        for (int j = Top + InterlacedOffset[i]; j < Top + Height; j += InterlacedJumps[i]) {
                            if (DGifGetLine(gf, &screen[j][Left], Width) == GIF_ERROR) {
                                PrintGifError();
                                for (int j = 0; j < gf->SHeight; j++) {
                                    delete screen[j];
                                }
                                delete screen;
                                return false;
                            }
                        }
                    }
                } else {
                    GRAPHICS_LOG("not Interlaced.\n");
                    for (int i = Top; i < Top + Height; i++) {
                        if (DGifGetLine(gf, &screen[i][Left], Width) == GIF_ERROR) {
                            PrintGifError();
                            for (int j = 0; j < gf->SHeight; j++) {
                                delete screen[j];
                            }
                            delete screen;
                            return false;
                        }
                    }
                }

                /* Get the color map */
                ColorMapObject* ColorMap = (gf->Image.ColorMap ? gf->Image.ColorMap : gf->SColorMap);
                if (ColorMap == NULL) {
                    fprintf(stderr, "Gif Image does not have a colormap\n");
                    for (int j = 0; j < gf->SHeight; j++) {
                        delete screen[j];
                    }
                    delete screen;
                    return false;
                }

                for (int i = 0; i < gf->SHeight; i++) {
                    for (int j = 0; j < gf->SWidth; j++) {
                        int offset = i * gf->SWidth * bit_depth + j * bit_depth;
                        GifColorType* color = &ColorMap->Colors[screen[i][j]];
                        m_image_buffer[offset + 0] = color->Red;
                        m_image_buffer[offset + 1] = color->Green;
                        m_image_buffer[offset + 2] = color->Blue;
                        //m_image_buffer[offset + 3] = 0xff;
                    }
                }
                rt = TERMINATE_RECORD_TYPE;
                break;
            }
        case EXTENSION_RECORD_TYPE:
            {
                int extcode = 0;
                GifByteType * buf = NULL;
                DGifGetExtension(gf, &extcode, &buf);
                GRAPHICS_LOG("extcode   = %d\n", extcode);
                //tools::dump(buf + 1, (int)buf[0]);
            }
            break;
        case TERMINATE_RECORD_TYPE:
            break;
        default:            /* Should be traps by DGifGetRecordType. */
            break;
        }
    } while (rt != TERMINATE_RECORD_TYPE);

    color_space = C_RGB;
    return true;
}/*}}}*/

void Thumbnail::save_jpeg(FILE* fp, int width, int height, int depth, int quality)
{/*{{{*/
    if (fp == NULL) {
        GRAPHICS_LOG_ERROR("fp is NULL!!\n");
        return;
    }

    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;

    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;                                                         
    if (setjmp(jerr.setjmp_buffer)) {
        GRAPHICS_LOG_ERROR("JPEG compression Err.\n");
        jpeg_destroy_compress(&cinfo);
        return ;
    }             

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = depth / 8;

    if (color_space == C_RGB)
        cinfo.in_color_space = JCS_RGB;
    else if (color_space == C_ARGB)
        cinfo.in_color_space = JCS_RGB;//JCS_ARGB;
    else if (color_space == C_GRAY)
        cinfo.in_color_space = JCS_GRAYSCALE;

    row_stride = width * depth / 8;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)m_dst_buffer.buffer() + cinfo.next_scanline * row_stride;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}/*}}}*/

void Thumbnail::resample(int dst_width, int dst_height, FILE* fp, int quality)
{/*{{{*/
    if (dst_width <= 0) {
        dst_width = m_width * dst_height / m_height;
    } else if (dst_height <= 0) {
        dst_height = m_height * dst_width / m_width;
    }
    GRAPHICS_LOG("resample: from (%d, %d) to (%d, %d). depth = %d.\n",
        m_width, m_height, dst_width, dst_height, m_depth);
    GRAPHICS_LOG("image size =%d\n", m_image_buffer.length());

    m_dst_buffer.assign(dst_width * dst_height * m_depth / 8);
    int src_bytes_per_line = m_width * m_depth / 8;
    int dst_bytes_per_line = dst_width * m_depth / 8;

    for (int j = 0; j < dst_height; j++) {
        for (int i = 0; i < dst_width; i++) {
            float mx = m_width * (i + 0.5f) / dst_width;
            float my = m_height * (j + 0.5f) / dst_height;
            if (mx > m_width - 1)
                mx = m_width - 1;
            if (my > m_height - 1)
                my = m_height - 1;

            if (interpolation == I_NEAREST) {
                // 1. nearest neighbour
                int x = mx + 0.5 - 1;
                int y = my + 0.5 - 1;
                int src_offset = src_bytes_per_line * y + x * m_depth / 8;
                int dst_offset = dst_bytes_per_line * j + i * m_depth / 8;
                for (int k = 0; k < m_depth / 8; k++) {
                    m_dst_buffer[dst_offset + k] = m_image_buffer[src_offset + k];
                }
            } else if (interpolation == I_LINEAR) {
                // 2. Linear
                int x1 = mx;
                int y1 = my;
                int x2 = x1 + 1;
                int y2 = y1 + 1;
                if (x2 >= m_width)
                    x2 = m_width - 1;
                if (y2 >= m_height)
                    y2 = m_height - 1;

                //DCHECK(x2 >= mx) << "x1 = " << x1 << " x2 = " << x2 << ", mx = " << mx;
                //DCHECK(mx >= x1) << "x1 = " << x1 << " x2 = " << x2 << ", mx = " << mx;

                int dst_offset = dst_bytes_per_line * j + i * m_depth / 8;
                int src_offset_tl = src_bytes_per_line * y1 + x1 * m_depth / 8;
                int src_offset_tr = src_bytes_per_line * y1 + x2 * m_depth / 8;
                int src_offset_bl = src_bytes_per_line * y2 + x1 * m_depth / 8;
                int src_offset_br = src_bytes_per_line * y2 + x2 * m_depth / 8;
                for (int k = 0; k < m_depth / 8; k++) {
                    unsigned char r1 = (float)(x2 - mx) * (unsigned char)m_image_buffer[src_offset_tl + k] + (float)(mx - x1) * (unsigned char)m_image_buffer[src_offset_tr + k];
                    unsigned char r2 = (float)(x2 - mx) * (unsigned char)m_image_buffer[src_offset_bl + k] + (float)(mx - x1) * (unsigned char)m_image_buffer[src_offset_br + k];
                    m_dst_buffer[dst_offset + k] = (y2 - my) * r1 + (my - y1) * r2;
                }
            } else {
                // 3. shrink
                // bicubic interpolation by Blake L. Carlson <blake-carlson(at)uiowa(dot)edu
                // 16个像素做卷积和
                int x = mx;
                int y = my;
                float u = mx - x;
                float v = my - y;

                float rgb[10];
                for (int k = 0; k < 10; k++)
                    rgb[k] = 0.0f;

                float total = 0.0f;

                for (int m = -1; m < 3; m++) {
                    for (int n = -1; n < 3; n++) {
                        float f1 = 0.0f;
                        float f2 = 0.0f; // bell_interpolation(v - n);

                        if (interpolation == I_TRIANGLE) {
                            f1 = triangle_interpolation(m - u);
                            f2 = triangle_interpolation(v - n);
                        } else if (interpolation == I_BELL) {
                            f1 = bell_interpolation(m - u);
                            f2 = bell_interpolation(v - n);
                        } else if (interpolation == I_BSP) {
                            f1 = bspline_interpolation(m - u);
                            f2 = bspline_interpolation(v - n);
                        } else {
                            f1 = catmullrom_interpolation(m - u);
                            f2 = catmullrom_interpolation(v - n);
                        }

                        total += f2 * f1;

                        int cx = x + n;
                        int cy = y + m;
                        if (cx >= m_width)
                            cx = m_width - 1;
                        if (cy >= m_height)
                            cy = m_height - 1;

                        int src_offset = src_bytes_per_line * cy + cx * m_depth / 8;
                        for (int k = 0; k < m_depth / 8; k++) {
                            rgb[k] += (unsigned char)m_image_buffer[src_offset + k] * f2 * f1;
                        }
                    }
                }

                // DCHECK(total == 1.0f) << total;
                int dst_offset = dst_bytes_per_line * j + i * m_depth / 8;
                for (int k = 0; k < m_depth / 8; k++) {
                    m_dst_buffer[dst_offset + k] += rgb[k] / total;
                }
            }
        }
    }
    save_jpeg(fp, dst_width, dst_height, m_depth, quality);
}/*}}}*/


float Thumbnail::triangle_interpolation(float f)
{
    // 双立方插值表达式
    // 三角: 
    // R(x) = { 
    //              x + 1       (-1 <= x <= 0) 
    //              1 - x       (0 < x <= 1)
    //        }
    f = f / 2.0;
    if (f < 0.0f) {
        return f + 1.0f;
    } else {
        return 1.0f - f;
    }
}

float Thumbnail::bell_interpolation(float f)
{
    // bell分布:
    // R(x) = {
    //              0.5 * (x + 1.5) ^ 2     (-1.5 <= x <= -0.5)
    //              0.75 - x ^ 2            (-0.5 < x <= 0.5)
    //              0.5 * (x - 1.5) ^ 2     (0.5 < x <= 1.5)
    //        }

    f = (f / 2.0f) * 1.5;

    if (f > -1.5f && f <= -0.5f) {  
        return (0.5f * powf(f + 1.5f, 2.0f));  
    } else if (f > -0.5f && f <= 0.5f) {  
        return 0.75f - f * f;  
    } else if ((f > 0.5f && f < 1.5f)) {  
        return (0.5 * powf(f - 1.5f, 2.0f));  
    }
    return 0.0;  
}

float Thumbnail::bspline_interpolation(float f)
{
    // B样条分布:
    // R(x) = 
    //              2 / 3 + 0.5 * abs(x ^ 3) - x ^ 2        (0 <= abs(x) <= 1)
    //              1 / 6 * (2 - abs(x)) ^ 3                (1 < abs(x) <= 2)

    if (f < 0.0)
        f = -f;

    if (f <= 1.0) {
        return (2.0f / 3.0f) + 0.5f * (f * f * f) - (f * f);
    } else if (f <= 2.0) {
        return 1.0f / 6.0f * powf((2.0f - f), 3.0f);
    }
    return 1.0;
}

float Thumbnail::catmullrom_interpolation(float f)
{
    const float B = 0.0f;
    const float C = 0.5f;

    if (f < 0.0)
        f = -f;

    if (f < 1.0) {  
        return ((12 - 9 * B - 6 * C) * (f * f * f) + (-18 + 12 * B + 6 *C) * (f * f) + (6 - 2 * B)) / 6.0;
    } else if (f >= 1.0 && f < 2.0) {
        return ((-B - 6 * C) * (f * f * f) + (6 * B + 30 * C) * (f * f) + (-(12 * B) - 48 * C) * f + 8 * B + 24 * C) / 6.0;
    } else {
        return 0.0;
    }
}

}
