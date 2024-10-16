/******************************************************************************
* Copyright © 2024-2035 Light Zhang <mapaware@hotmail.com>, MapAware, Inc.
* ALL RIGHTS RESERVED.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/
/**
 * @file drawshape.h
 * @brief draw shape file on to a png file.
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.9
 *
 * @since 2024-10-13 22:24:17
 * @date 2024-10-16 22:41:24
 *
 * @note
 *   https://github.com/pepstack/shapefile
 *
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


#ifdef    __cplusplus
}
#endif
#endif /* DRAW_SHAPE_H__ */