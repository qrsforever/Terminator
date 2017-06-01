#include "GraphicsAssertions.h"
#include "LCDLayer.h"
#include "Canvas.h"

#include "cairo-ft.h"
#include "freetype/freetype.h"

#include "libzebra.h"

extern FT_Face share_ft_face;

namespace IBox {

LCDLayer::LCDLayer()
    : mWidth(320), mHeight(240)
{
    unsigned char *buffer = 0;                       
    int pitch = 0;    

    yhw_lcd_setBackLight(1);
    yhw_lcd_createLayer();
    yhw_lcd_getLayer(&mHandle);
    ygp_layer_getCreateSize(mHandle, &mWidth, &mHeight);
    ygp_layer_getMemory(mHandle, (int *)&buffer, &pitch);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_RGB16_565, mWidth, mHeight, pitch);

    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy (surface);

    cairo_font_face_t *face = 0; 
    face = cairo_ft_font_face_create_for_ft_face(share_ft_face, 0);
    cairo_set_font_face(cr, face);

    mCanvas = new Canvas(cr);
}

LCDLayer::~LCDLayer()
{
}

void
LCDLayer::show()
{
    return;
}

void
LCDLayer::hide()
{
    return;
}

LCDLayer& 
LCDLayer::self()
{
    static LCDLayer* sLayer; 
    if (!sLayer) {
       sLayer = new LCDLayer(); 
       sLayer->show();
    }
    return *sLayer;
}

} // namespace IBox
