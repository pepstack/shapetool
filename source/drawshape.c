/**
 * @file drawshape.c
 * @author 350137278@qq.com
 * @brief draw shape file on to a png file
 * @version 0.0.5
 * @date 2024-10-14 01:12:06
 *
 * @copyright Copyright (c) 2024, mapaware.top
 *
 * @note
 *     https://github.com/pepstack/shapefile
 *
 */
#include "shapetool-common.h"
#include "drawshape.h"


int shpfile2png(shapetool_flags *flags, shapetool_options *options)
{
    shapeFileInfo shpInfo;
    cairoDrawCtx CDC;
    cairo_status_t status;

    // load shp file: file://
    if (shapeFileInfoOpen(&shpInfo, CSTR_FILE_URI_PATH(options->shpfile)) != 0) {
        return -1;
    }

    // get stype classes
    if (flags->stylecss) {
        // if css file provided, check css class
        if (! flags->styleclass) {
            // if not class given , use default class name depenps on the type of shape
            if (shpInfo.nShpTypeMask == SHAPE_TYPE_POLYGON) {
                options->styleclass = cstrbufDup(options->styleclass, ".polygon", 8);
            } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_LINE) {
                options->styleclass = cstrbufDup(options->styleclass, ".line", 5);
            } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_POINT) {
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
        return -1;
    }

    // parse css file and config draw style
    cairoDrawCtxSetCssStyle(&CDC, CBSTR(options->stylecss), CBSTR(options->styleclass));

    // draw shapes onto cairo
    shapeFileInfoDraw(&shpInfo, &CDC);

    status = cairoDrawCtxOutputPng(&CDC, 0, CSTR_FILE_URI_PATH(options->outpng));

    cairoDrawCtxFinal(&CDC);

    shapeFileInfoClose(&shpInfo);

    return status;
}