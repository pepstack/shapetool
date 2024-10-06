/**
 * @file drawshape.c
 * @author 350137278@qq.com
 * @brief draw shape file on to a png file
 * @version 0.0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024, mapaware.top
 * 
 * @note
 *     https://github.com/pepstack/shapefile
 * 
 */
#include "shapetool-common.h"
#include "drawshape.h"


int shpfile2png(const shapetool_options *options)
{
    shapeFileInfo shpInfo;
    cairoDrawCtx CDC;
    cairo_status_t status;
    CssClassGroup styleClasses;

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

/*
    styleClasses.classnum =
        cstr_split_multi_chrs(cstrbufGetStr(options->styleclass), cstrbufGetLen(options->styleclass),
            ",;", 2,
            styleClasses.classes, styleClasses.namelens,
            sizeof(styleClasses.classes)/sizeof(styleClasses.classes[0]));

    if (styleClasses.classnum == 0)
        // set default class
        if (shpInfo.nShpTypeMask == SHAPE_TYPE_POLYGON) {
            styleClasses.classes[0] = mem_strdup_len("polygon", 7);
            styleClasses.namelens[0] = 7;
        } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_LINE) {
            styleClasses.classes[0] = mem_strdup_len("line", 4);
            styleClasses.namelens[0] = 4;
        } else if (shpInfo.nShpTypeMask == SHAPE_TYPE_POINT) {
            styleClasses.classes[0] = mem_strdup_len("point", 5);
            styleClasses.namelens[0] = 5;
        } else {
            styleClasses.classes[0] = mem_strdup_len("*", 1);
            styleClasses.namelens[0] = 1;
        }
        styleClasses.classnum++;
    }

    ParseCssStyle(cstrbufGetStr(options->style_cssfile), &styleClasses);
*/

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

    if (cairoDrawCtxInit(&CDC, dataBox, viewSize, dot_logical_px, options->dpi)) {
        shapeFileInfoClose(&shpInfo);
        // katana_destroy_output(cssStyle);
        return -1;
    }

    // draw shapes onto canvas
    shapeFileInfoDraw(&shpInfo, &CDC);

    //katana_destroy_output(cssStyle);

    status = cairoDrawOutputPng(&CDC, 0, cstrbufGetStr(options->outpng));

    cairoDrawCtxFinal(&CDC);

    shapeFileInfoClose(&shpInfo);

    return status;
}