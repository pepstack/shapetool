/**
 * Copyright © 2024 MapAware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author: Light Zhang <mapaware.top>
 * @date 2024-10-15 02:03:55
 */
#ifndef CAIRO_MAP_CANVAS_H__
#define CAIRO_MAP_CANVAS_H__

#ifdef    __cplusplus
extern "C" {
#endif



#ifdef WIN32API
#   if !defined(__MINGW__)
    // cairo 2d draw api
    #include "cairo.h"
    #include "cairo-features.h"
    #include "cairo-svg.h"
    #include "cairo-pdf.h"
    #include "cairo-ps.h"
    #include "cairo-version.h"
    #include "cairo-win32.h"
#   else
    #include <cairo/cairo.h>
    #include <cairo/cairo-features.h>
    #include <cairo/cairo-svg.h>
    #include <cairo/cairo-pdf.h>
    #include <cairo/cairo-ps.h>
    #include <cairo/cairo-version.h>
    #include <cairo/cairo-win32.h>
#   endif
#else
    // Linux
    // cairo 2d draw api
    #include <cairo/cairo.h>
    #include <cairo/cairo-features.h>
    #include <cairo/cairo-svg.h>
    #include <cairo/cairo-pdf.h>
    #include <cairo/cairo-ps.h>
    #include <cairo/cairo-version.h>
    #include <cairo/cairo-win32.h>
#endif


#include <common/viewport.h>

#include "cssdrawstyle.h"


#ifndef CAIRO_DRAW_WIDTH_MAX
#   define CAIRO_DRAW_WIDTH_MAX       16384
#   define CAIRO_DRAW_HEIGHT_MAX      16384
#endif

#ifndef CAIRO_DRAW_WIDTH_MIN
#   define CAIRO_DRAW_WIDTH_MIN       128
#   define CAIRO_DRAW_HEIGHT_MIN      96
#endif

#ifndef CAIRO_DRAW_WIDTH_DEFAULT
// default 15.6 in, 4K display
#   define CAIRO_DRAW_WIDTH_DEFAULT   3840
#   define CAIRO_DRAW_HEIGHT_DEFAULT  2160
#endif


typedef enum
{
    dot_logical_px,         // dot (logical pixel)
    dot_device_pt,          // pt (1/72 inch)
    dot_unit_mm,            // mm (1/1000 meter)
    dot_unit_cm,            // cm (1/100 meter)
    dot_unit_in,            // inch (1 in. = 25.4 mm)
} cairoDotUnit;


typedef enum
{
    dpi_screen_display = 0,
    dpi_draft_display = 72,    // px same as pt (1/72 inch)
    dpi_low_display = 96,
    dpi_med_display = 150,
    dpi_high_display = 300,
    dpi_low_print = dpi_high_display,
    dpi_med_print = 600,
    dpi_high_print = 1200
} cairoDrawDPI;


typedef struct
{
    // cairo paint devices
    cairo_surface_t *surface;
    cairo_t *cr;

    ///CssPolygonStyle  polygonStyle;

    // paint viewport
    Viewport2D viewport;

    CssDrawStyle drawStyles;
} cairoDrawCtx;


static int cairoDrawCtxInit(cairoDrawCtx *CDC, CGBox2D dataBox, CGSize2D drawSize, cairoDotUnit dotUnit, float drawDPI)
{
    CGBox2D viewBox = {
        .Xmin = 0,
        .Ymin = 0,
        .Xmax = drawSize.W,
        .Ymax = drawSize.H
    };

    CGSize2D viewDPI = {drawDPI, drawDPI};

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)drawSize.W, (int)drawSize.H);
    if (! surface) {
        printf("Error: cairo_image_surface_create()\n");
        return -1;
    }

    CDC->cr = cairo_create(surface);
    if (! CDC->cr) {
        printf("Error: cairo_create()\n");
        cairo_surface_destroy(surface);
        return -1;
    }
    CDC->surface = surface;

    ViewportInitAll(&CDC->viewport, dataBox, viewBox, viewDPI, 1.0f);

    // TODO:
    ///CDC->polygonStyle.border_color.red = 128;
    ///CDC->polygonStyle.fill_color.blue = 128;

    return 0;
}


static void cairoDrawCtxFinal(cairoDrawCtx *CDC)
{
    cairo_surface_t *surface = CDC->surface;
    cairo_t *cr = CDC->cr;

    CDC->surface = 0;
    CDC->cr = 0;

    if (cr) {
        cairo_destroy(cr);
    }
    if (surface) {
        cairo_surface_destroy(surface);
    }
}


static void cairoDrawCtxSetStyle(cairoDrawCtx *CDC, const CssKeyArray cssStyleKeys, cstrbuf styleClass)
{
    CssKeyArrayNode classNodes[32] = { 0 };
    int Nodes = CssKeyArrayQueryClass(cssStyleKeys, css_type_class, styleClass->str, styleClass->len, classNodes);

    // TODO: set CDC with style


}


static cairo_status_t cairoDrawCtxOutputPng(cairoDrawCtx *CDC, const CGBox2D *viewBoxOutput, const char * outputPngFile)
{
    cairo_status_t status = CAIRO_STATUS_LAST_STATUS;

    if (! viewBoxOutput) {
        status = cairo_surface_write_to_png(CDC->surface, outputPngFile);
    } else {
        // TODO:
    }

    return status;
}


#ifdef    __cplusplus
}
#endif
#endif /* CAIRO_MAP_CANVAS_H__ */