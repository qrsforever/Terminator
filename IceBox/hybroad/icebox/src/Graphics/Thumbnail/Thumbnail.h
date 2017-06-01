#ifndef __Thumbnail__H_
#define __Thumbnail__H_ 

#include <string>
#include <stdio.h>
#include "Buffer.h"

namespace IBox {

class Thumbnail
{
public:
    Thumbnail();
    ~Thumbnail();

    enum {
        OPER_NNN = -1,
        OPER_CUTIMG = 0,
        OPER_FILLIMG = 1,
    };

    typedef enum {
        R_SUCCESS,
        R_READ_FAILED,
        R_WRITE_FAILED,
        R_CANNOT_RECOGNIZE,
        R_NOT_SUPPORT,
        R_CUT_FAILED,
        R_FILL_FAILED,
    } RESULT;
    /***************
    * inpath  : 源文件路径
    * outpath : 生成缩略图文件路径
    * quality : 表示压缩比率，0～100之间的整数
    * maxSize : 表示缩略图最大尺寸
    *****************/

    RESULT create(const char * inpath, const char * outpath, int quality = 80, int maxSize = 150);
    RESULT createBySize(const char * inpath, const char * outpath, int wsize, int hsize, int quality = 20, int maxImgSize = 2000 /*byte*/);
    RESULT createByScale(const char * inpath, const char * outpath, float scale, int quality = 80, int maxImgSize = 1024000 /*byte*/);

    typedef enum {
        I_NEAREST,
        I_LINEAR,
        I_TRIANGLE,
        I_BELL,
        I_BSP,
        I_CATMULLROM,
    } INTERPOLATION;
    void set_interpolation_type(INTERPOLATION interpolation);

private:
    int loadImage(const char * inpath);
    bool load_jpeg(FILE* fp);
    bool load_png(FILE* fp);
    bool load_gif(const char * path);
    bool load_bmp(FILE* fp);
    bool cut_squareimg();
    bool fill_squareimg(int color);

    void save_jpeg(FILE* fp, int width, int height, int depth, int quality);
    void resample(int dst_width, int dst_height, FILE* fp, int quality);

    // 双立方插值表达式
    // 三角: 
    // R(x) = { 
    //              x + 1       (-1 <= x <= 0) 
    //              1 - x       (0 < x <= 1)
    //        }
    float triangle_interpolation(float f);

    // bell分布:
    // R(x) = {
    //              0.5 * (x + 1.5) ^ 2     (-1.5 <= x <= -0.5)
    //              0.75 - x ^ 2            (-0.5 < x <= 0.5)
    //              0.5 * (x - 1.5) ^ 2     (0.5 < x <= 1.5)
    //        }
    float bell_interpolation(float f);

    // B样条分布:
    // R(x) = {
    //              2 / 3 + 0.5 * abs(x ^ 3) - x ^ 2        (0 <= abs(x) <= 1)
    //              1 / 6 * (2 - abs(x)) ^ 3                (1 < abs(x) <= 2)
    float bspline_interpolation(float f);

    // cat mull rom 
    float catmullrom_interpolation(float f);

    tools::Buffer   m_image_buffer;
    tools::Buffer   m_dst_buffer;

    int m_width;
    int m_height;
    int m_depth;
    int m_oper;

    INTERPOLATION   interpolation;

    enum {
        C_GRAY,
        C_RGB,
        C_ARGB,
    } color_space;

};

}

#endif
