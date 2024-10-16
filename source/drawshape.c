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
 * @file drawshape.c
 * @brief draw shape file on to a png file.
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.9
 *
 * @since 2024-10-15 01:33:13
 * @date 2024-10-17 03:02:02
 *
 * @note
 *   https://github.com/pepstack/shapefile
 *
 */
#include "shapetool-common.h"
#include "drawshape.h"


int shpfile2png(shapetool_flags *flags, shapetool_options *options)
{
    shapeFileInfo shpInfo;
    cairoDrawCtx CDC;
    cairo_status_t status;

    // load shp file: file:///path/to/some.shp
    if (shapeFileInfoOpen(&shpInfo, CSTR_FILE_URI_PATH(options->shpfile)) != 0) {
        return SHAPETOOL_RES_ERR;
    }

    // get stype classes
    if (flags->style) {
        // if css file provided, check css class
        if (!flags->styleclass) {
            // if not class given , set default class name by type of shape
            if (shpInfo.nShpTypeMask == SHAPE_TYPE_POLYGON) {
                options->styleclass = cstrbufDup(options->styleclass, ".polygon", 8);
            }
            else if (shpInfo.nShpTypeMask == SHAPE_TYPE_LINE) {
                options->styleclass = cstrbufDup(options->styleclass, ".line", 5);
            }
            else if (shpInfo.nShpTypeMask == SHAPE_TYPE_POINT) {
                options->styleclass = cstrbufDup(options->styleclass, ".point", 6);
            }
            flags->styleclass = 1;
        }
    }

    // create cairo draw context
    CGBox2D dataBox = {
        .Xmin = shpInfo.minBounds[0],
        .Ymin = shpInfo.minBounds[1],
        .Xmax = shpInfo.maxBounds[0],
        .Ymax = shpInfo.maxBounds[1]
    };
    CGSize2D viewSize = {
        .W = options->width,
        .H = options->height
    };

    if (cairoDrawCtxInit(&CDC, dataBox, viewSize, dot_logical_px, (float)options->dpi)) {
        shapeFileInfoClose(&shpInfo);
        exit(1);
    }

    if (flags->style && flags->styleclass) {
        // set draw context with css style
        cairoDrawCtxSetStyle(&CDC, options->cssStyleKeys, options->styleclass);
    }

    // draw shapes onto cairo
    shapeFileInfoDraw(&shpInfo, &CDC);

    status = cairoDrawCtxOutputPng(&CDC, 0, CSTR_FILE_URI_PATH(options->outpng));

    cairoDrawCtxFinal(&CDC);

    shapeFileInfoClose(&shpInfo);

    if (status != CAIRO_STATUS_SUCCESS) {
        return SHAPETOOL_RES_ERR;
    }

    return SHAPETOOL_RES_SOK;
}


void drawPolygonShape(const SHPObjectEx* hShpRef, cairoDrawCtx* cdc)
{
    int i, part, cnt;
    double X0, Y0, X, Y;

    SHPPointType* points, * ppt;

    cairo_t* cr = cdc->cr;
    Viewport2D* vwp = &(cdc->viewport);

    ///CssPolygonStyle * polygon = &cdc->polygonStyle;

    cairo_save(cr);

    for (part = 0; part < hShpRef->nParts; part++) {
        /* start index of points of current part */
        int start = hShpRef->panPartStart[part];

        /* type of current part */
        int parttype = hShpRef->panPartType[part];

        /* number of points of part */
        int npp = hShpRef->panPartStart[part + 1] - start;

        if (npp > 0) {
            points = &hShpRef->pPoints[start];

            if (part == 0) {
                /* first part as contour path */
                cairo_new_path(cr);
            }
            else {
                /* hole path */
                cairo_new_sub_path(cr);
            }

            i = cnt = 0;
            ppt = &points[i++];

            DataToViewXY(vwp, ppt->x, ppt->y, &X0, &Y0);

            cairo_move_to(cr, X0, Y0);

            while (i < npp) {
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
