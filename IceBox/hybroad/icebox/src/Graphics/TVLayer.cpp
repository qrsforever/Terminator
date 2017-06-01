#include "GraphicsAssertions.h"
#include "TVLayer.h"
#include "Canvas.h"

#include "libzebra.h"

#include "ft2build.h"
#include "cairo-ft.h"
#include "freetype/freetype.h"

extern FT_Face share_ft_face;

#define MAX_SCREEN_WIDTH 1280
#define MAX_SCREEN_HEIGHT 720

namespace IBox {

TVLayer::TVLayer()
    : mWidth(MAX_SCREEN_WIDTH), mHeight(MAX_SCREEN_HEIGHT)
{
    unsigned char *buffer = 0;                       
    int pitch = 0;    

    ygp_layer_createGraphics(mWidth, mHeight, YX_COLOR_ARGB8888, &mHandle);
    ygp_layer_getMemory(mHandle, (int *)&buffer, &pitch);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, mWidth, mHeight, pitch);

    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy (surface);

    cairo_font_face_t *face = 0; 
    face = cairo_ft_font_face_create_for_ft_face(share_ft_face, 0);
    cairo_set_font_face(cr, face);

    mCanvas = new Canvas(cr);
}

TVLayer::~TVLayer()
{
}

void
TVLayer::show()
{
    ygp_layer_setShow(mHandle, 1);
}

void
TVLayer::hide()
{
    ygp_layer_setShow(mHandle, 0);
}

TVLayer& 
TVLayer::self()
{
    static TVLayer* sLayer; 
    if (!sLayer) {
       sLayer = new TVLayer(); 
       sLayer->show();
    }
    return *sLayer;
}

} // namespace IBox
