/**
 * @file drawshape.c
 * @author 350137278@qq.com
 * @brief draw shape file on to a png file
 * @version 0.0.3
 * @date 2024-10-10 20:39:00
 *
 * @copyright Copyright (c) 2024, mapaware.top
 *
 * @note
 *     https://github.com/pepstack/shapefile
 *
 */
#include "shapetool-common.h"
#include "drawshape.h"


int shpfile2png(shapetool_options *options)
{
    shapeFileInfo shpInfo;
    cairoDrawCtx CDC;
    cairo_status_t status;

    if (! options->shpfile) {
        return -1;
    }
    if (! options->outpng) {
        return -1;
    }

    printf("Info: %s: %s => %s\n", __FUNCTION__, cstrbufGetStr(options->shpfile), cstrbufGetStr(options->outpng));

    // load shp file
    if (shapeFileInfoOpen(&shpInfo, cstrbufGetStr(options->shpfile)) != 0) {
        return -1;
    }

    // get stype classes
    if (cstrbufGetStr(options->stylecss)) {
        if (! cstrbufGetStr(options->styleclass)) {
            if (shpInfo.nShpTypeMask == SHAPE_TYPE_POLYGON) {
                options->styleclass = cstrbufDup(options->styleclass, ".polygon", 7);
            } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_LINE) {
                options->styleclass = cstrbufDup(options->styleclass, ".line", 5);
            } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_POINT) {
                options->styleclass = cstrbufDup(options->styleclass, ".point", 6);
            }
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
        return -1;
    }

    // parse css file and config draw style
    cairoDrawCtxSetCssStyle(&CDC, cstrbufGetStr(options->stylecss), cstrbufGetStr(options->styleclass));

    // draw shapes onto cairo
    shapeFileInfoDraw(&shpInfo, &CDC);

    status = cairoDrawCtxOutputPng(&CDC, 0, cstrbufGetStr(options->outpng));

    cairoDrawCtxFinal(&CDC);

    shapeFileInfoClose(&shpInfo);

    return status;
}