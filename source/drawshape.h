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
 * @date 2024-10-13 22:24:17
 */
#ifndef DRAW_SHAPE_H__
#define DRAW_SHAPE_H__

#ifdef    __cplusplus
extern "C" {
#endif

// Build libshapefile.so:
//  $ cd deps/
//  $ git clone https://github.com/pepstack/shapefile
//  $ cd shapefile
//  $ make
#include <shapefile/shapefile_api.h>

// draw context
#include "cairodrawctx.h"


typedef struct
{
    SHPHandle hSHP;
    DBFHandle hDBF;

    int nEntities;
    int nShapeType;
    int nShpTypeMask;

    // Xmin, Ymin, Zmin, Mmin
    double minBounds[4];
	double maxBounds[4];

    int hasZ;
    int hasM;

    char shapefile[256];
} shapeFileInfo;


void drawPolygonShape(const SHPObjectEx *hShpRef, cairoDrawCtx *cdc);


static void shapeFileInfoClose(shapeFileInfo *shpInfo)
{
    DBFClose(shpInfo->hDBF);
    SHPClose(shpInfo->hSHP);
}


static int shapeFileInfoOpen(shapeFileInfo *shpInfo, const char *shapefile)
{
    bzero(shpInfo, sizeof(shapeFileInfo));

    shpInfo->hSHP = SHPOpen(shapefile, "rb");
    if (! shpInfo->hSHP) {
        printf("Error: Cannot open shp file: %s\n", shapefile);
        return -1;
    }

    shpInfo->hDBF = DBFOpen(shapefile, "rb" );
    if (! shpInfo->hDBF) {
        SHPClose(shpInfo->hSHP);
        return -1;
    }

	SHPGetInfo(shpInfo->hSHP, &shpInfo->nEntities, &shpInfo->nShapeType, shpInfo->minBounds, shpInfo->maxBounds);

    shpInfo->nShpTypeMask = SHPGetType(shpInfo->hSHP, &shpInfo->hasZ, &shpInfo->hasM);
    if (shpInfo->nShpTypeMask == SHAPE_TYPE_NIL) {
        printf("Error: Bad shp type: SHAPE_TYPE_NIL\n");
        shapeFileInfoClose(shpInfo);
        return -1;
    }

    // All success
    return 0;
}


static void shapeFileInfoDraw(shapeFileInfo *shpInfo, cairoDrawCtx *CDC)
{
    int nShapeId;

    CGBox2D shapeEnv;   // data rect
    CGBox2D drawRect;    // draw rect

    SHPObjectEx * shapeReadRef = 0;
    if (! SHPCreateObjectEx(&shapeReadRef)) {
        // out of memory
        abort();
    }

    int nShpTypeMask = shpInfo->nShpTypeMask;

    for (nShapeId = 0; nShapeId < shpInfo->nEntities; nShapeId++) {
        // read bounding rect of shape
        if (SHPReadObjectEnvelope(shpInfo->hSHP, nShapeId, (SHPEnvelope *) &shapeEnv, 0) != SHPT_NULL) {
            // convert to canvas box
            DataToViewBox(&CDC->viewport, shapeEnv, &drawRect);

            // test if overlapped of canvas with shape
            if (CGBoxIsOverlap(CDC->viewport.viewBox, drawRect)) {
                if (nShpTypeMask == SHAPE_TYPE_POLYGON) {
                    if (CGBoxGetDX(drawRect) > 0 && CGBoxGetDY(drawRect) > 0) {
                        // polygon shape is visible
                        if (SHPReadObjectEx(shpInfo->hSHP, nShapeId, shapeReadRef)) {
                            drawPolygonShape(shapeReadRef, CDC);
                        } else {
                            printf("Warn: SHPReadObjectEx() failed on shape#%d\n", nShapeId);
                        }
                    }
                } else if (nShpTypeMask == SHAPE_TYPE_LINE) {

                } else if (nShpTypeMask == SHAPE_TYPE_POINT) {

                }
            }
        }
    }

    SHPDestroyObjectEx(shapeReadRef);
}


void drawPolygonShape(const SHPObjectEx *hShpRef, cairoDrawCtx *cdc)
{
    int i, part, cnt;
    double X0, Y0, X, Y;

    SHPPointType  *points, *ppt;

    cairo_t *cr = cdc->cr;
    Viewport2D *vwp = &(cdc->viewport);

    ///CssPolygonStyle * polygon = &cdc->polygonStyle;

    cairo_save(cr);

    for (part = 0; part < hShpRef->nParts; part++) {
        /* start index of points of current part */
        int start = hShpRef->panPartStart[part];

        /* type of current part */
        int parttype = hShpRef->panPartType[part];

        /* number of points of part */
        int npp = hShpRef->panPartStart[part+1] - start;

        if (npp > 0) {
            points = &hShpRef->pPoints[start];

            if (part == 0) {
                /* first part as contour path */
                cairo_new_path(cr);
            } else {
                /* hole path */
                cairo_new_sub_path(cr);
            }

            i = cnt = 0;
            ppt = &points[i++];

            DataToViewXY(vwp, ppt->x, ppt->y, &X0, &Y0);

            cairo_move_to(cr, X0, Y0);

            while(i < npp) {
                ppt = &points[i++];

                DataToViewXY(vwp, ppt->x, ppt->y, &X, &Y);

                if (CGPointNotEqual(X, Y, X0, Y0, 0.5)) {
                    cairo_line_to(cr, X, Y);
                    X0 = X;
                    Y0 = Y;
                }
            }

            cairo_close_path(cr);
        }
        else {
            // empty path
        }
    }

    if (part) {
        // success returns 0
        /// cairo_set_source_rgb(cr, polygon->fill_color.red, polygon->fill_color.green, polygon->fill_color.blue);
        cairo_set_source_rgb(cr, 128, 0, 128);
        cairo_fill_preserve(cr);

        ///cairo_set_source_rgb(cr, polygon->border_color.red, polygon->border_color.green, polygon->border_color.blue);
        cairo_set_source_rgb(cr, 0, 160, 35);
        cairo_stroke(cr);
    }
    else {
        // empty shape
    }

    cairo_restore(cr);
}


#ifdef    __cplusplus
}
#endif
#endif /* DRAW_SHAPE_H__ */