#include <stdio.h>
#include <unistd.h>
#include "cairo.h"
#include "ft2build.h"
#include "cairo-ft.h"
#include <math.h>
#include "freetype/freetype.h"

#define LINK_SDK 1

#if LINK_SDK
#include "libzebra.h"
#endif

int main(int argc, char** argv) 
{
    if (argc != 3) {
        return 1;
    }

    const char* input = argv[1];
    const char* output = argv[2];

    int handle;
    int width = 320;
    int height = 240;
    int pitch = width * 4;    
    unsigned char *buffer = 0;                       
#if LINK_SDK
    yhw_board_init();
    ygp_layer_createGraphics(width, height, YX_COLOR_ARGB8888, &handle);
    ygp_layer_getMemory(handle, (int *)&buffer, &pitch);
    ygp_layer_setShow(handle, 1);  
#else 
    buffer = (unsigned char*)malloc(height * pitch);
#endif

    cairo_surface_t *surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, width, height, pitch);

    memset(buffer, 0xff, height * pitch);
    cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy (surface);

    cairo_surface_t *image;
    image = cairo_image_surface_create_from_png(input);
    cairo_scale(cr, 0.5, 0.5);
    cairo_rotate(cr, 0.5);
    cairo_set_source_surface(cr, image, 0, 0);

    cairo_paint (cr);
    cairo_surface_destroy (image);

#if LINK_SDK
    ygp_layer_updateScreen(); 
#else 
    cairo_surface_write_to_png(cairo_get_target(cr), output);
#endif
    return 0;
}


