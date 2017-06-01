#include "GraphicsAssertions.h"
#include "PictureGIF.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "gif_lib.h"

static int
	BackGround = 0,
	InterlacedOffset[] = { 0, 4, 2, 1 }, /* The way Interlaced image should. */
	InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
static ColorMapObject *ColorMap;

namespace IBox {

PictureGIF::PictureGIF(const char* filepath) : Picture(filepath)
{

}

PictureGIF::~PictureGIF()
{

}

int 
PictureGIF::load()
{
    reset();
    int	i, j, gifSize, gifRow, gifCol, gifWidth, gifHeight, extCode;
    int frameNo = 0, tranColor=-1;
    GifRecordType recType;
    GifByteType *extension;
    GifRowType *screenBuff = 0;
    GifRowType *tmpBuff = 0;	// First frame save to screenBuff, others save here to be discarded
    GifFileType *gifFile;

    uint8_t *bits = 0, r, g, b, a;
    int width, height, stride;
    int *row = 0, *pixel = 0;

    if ((gifFile = DGifOpenFileName(mFilePath.c_str())) == NULL) {
        PrintGifError();
        goto Err;
    }
    /* Allocate the screen as vector of column of rows. We cannt allocate    */
    /* the all screen at once, as this broken minded CPU can allocate up to  */
    /* 64k at a time and our image can be bigger than that:		     */
    /* Note this screen is device independent - its the screen as defined by */
    /* the GIF file parameters itself.					     */
    screenBuff = (GifRowType *) malloc(gifFile->SHeight * sizeof(GifRowType*));
    if (!screenBuff)
        goto Err;
    tmpBuff = (GifRowType *) malloc(gifFile->SHeight * sizeof(GifRowType*));
    if (!tmpBuff)
        goto Err;

    gifSize = gifFile->SWidth * sizeof(GifPixelType);/* size in bytes one row.*/
    if ((screenBuff[0] = (GifRowType) malloc(gifSize)) == NULL) /* First row. */
        goto Err;
    if ((tmpBuff[0] = (GifRowType)malloc(gifSize)) == NULL)
        goto Err;

    GRAPHICS_LOG("sizeof(GifRowType *)=%d, sizeof(GifPixelType)=%d\n", sizeof(GifRowType *), sizeof(GifPixelType));

    for (i = 0; i < gifFile->SWidth; i++)  /* Set its color to BackGround. */
        screenBuff[0][i] = gifFile->SBackGroundColor;

    for (i = 1; i < gifFile->SHeight; i++) {
        /* Allocate the other rows, and set their color to background too: */
        screenBuff[i] = (GifRowType) malloc(gifSize);
        if (!screenBuff[i])
            goto Err;
        memcpy(screenBuff[i], screenBuff[0], gifSize);
    }

    /* Scan the content of the GIF file and load the image(s) in: */
    do {
        if (DGifGetRecordType(gifFile, &recType) == GIF_ERROR) {
            PrintGifError();
            goto Err;
        }
        switch (recType) {
        case IMAGE_DESC_RECORD_TYPE:
            if (DGifGetImageDesc(gifFile) == GIF_ERROR) {
                PrintGifError();
                goto Err;
            }
            gifRow = gifFile->Image.Top; /* Image Position relative to Screen. */
            gifCol = gifFile->Image.Left;
            gifWidth = gifFile->Image.Width;
            gifHeight = gifFile->Image.Height;
            if (gifFile->Image.Left + gifFile->Image.Width > gifFile->SWidth ||
                gifFile->Image.Top + gifFile->Image.Height > gifFile->SHeight) {
                GRAPHICS_LOG_ERROR("Image is not confined to screen dimension, aborted.\n");
                goto Err;
            }
            if (gifFile->Image.Interlace) {
                /* Need to perform 4 passes on the images: */
                for (i = 0; i < 4; i++)
                    for (j = gifRow + InterlacedOffset[i]; j < gifRow + gifHeight;
                        j += InterlacedJumps[i]) {
                        if (DGifGetLine(gifFile, frameNo? &tmpBuff[0][0]: &screenBuff[j][gifCol],
                                gifWidth) == GIF_ERROR) {
                            PrintGifError();
                            goto Err;;
                        }
                    }
            }
            else {
                for (i = 0; i < gifHeight; i++) {
                    if (DGifGetLine(gifFile, frameNo? &tmpBuff[0][0]: &screenBuff[gifRow++][gifCol],
                            gifWidth) == GIF_ERROR) {
                        PrintGifError();
                        goto Err;
                    }
                }
            }
            frameNo++;
            break;
        case EXTENSION_RECORD_TYPE:
            /* Skip any extension blocks in file: */
            if (DGifGetExtension(gifFile, &extCode, &extension) == GIF_ERROR) {
                PrintGifError();
                goto Err;
            }
            if ((!frameNo) && extension && (extCode == GRAPHICS_EXT_FUNC_CODE))
                if (*(extension + 1) & 1) /* Have transparent color */
                    tranColor = *(extension + *extension);
            while (extension != NULL) {
                if (DGifGetExtensionNext(gifFile, &extension) == GIF_ERROR) {
                    PrintGifError();
                    goto Err;
                }
            }
            break;
        case TERMINATE_RECORD_TYPE:
            break;
        default:		    /* Should be traps by DGifGetrecType. */
            break;
        }
    } while (recType != TERMINATE_RECORD_TYPE);

    /* Lets dump it - set the global variables required and do it: */
    BackGround = gifFile->SBackGroundColor;
    ColorMap = (gifFile->Image.ColorMap ? gifFile->Image.ColorMap : gifFile->SColorMap);
    if (ColorMap == NULL) {
        GRAPHICS_LOG_ERROR("Gif Image does not have a colormap\n");
        goto Err;
    }

    width = gifFile->SWidth;
    height = gifFile->SHeight;
    stride = width * 4;
    bits = (uint8_t*)malloc(stride * height);
    {
        GifColorType *tColorEntry;
        GifRowType tGifRow;
        for (i = 0; i < height; i++) {
            row = (int*)(bits + i * stride);
            tGifRow = screenBuff[i];
            for (j = 0; j < width; j++) {
                if ((tranColor > -1) && (tGifRow[j] == tranColor)&&(gifFile->SBackGroundColor!=0))
                    tColorEntry = &ColorMap->Colors[gifFile->SBackGroundColor];
                else
                    tColorEntry = &ColorMap->Colors[tGifRow[j]];
                pixel = row + j;
                r = tColorEntry->Blue;
                g = tColorEntry->Green;
                b = tColorEntry->Red;
                a = 0xff;
                *pixel = a << 24 | b << 16 | g << 8 | r;
            }
        }
    }
    for (i=0; i<gifFile->SHeight; i++)
        free(screenBuff[i]);

    if (DGifCloseFile(gifFile) == GIF_ERROR) {
        PrintGifError();
        goto Err;
    }

    mWidth = width;
    mHeight = height;
    mBitmap = bits;
    mBitDepth = 32;
    mColorSpace = C_ARGB;
    mStride = stride;
    mPicType = PIC_GIF;
    return 0;
Err:
    if (bits)
        free(bits);
    if (screenBuff)
        free(screenBuff);
    if (tmpBuff) {
        if (tmpBuff[0])
            free(tmpBuff[0]);
        free(tmpBuff);
    }
    return -1;
}

}
