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
    KatanaOutput * styleCSS = 0;

    if (! options->shpfile) {
        return -1;
    }
    if (! options->outpng) {
        return -1;
    }

    if (options->style) {
        if (cstr_startwith(options->style->str, options->style->len, "file://", 7)) {
            FILE *fp = fopen(&options->style->str[7], "r");
            if (! fp) {
                printf("Error: Cannot open file: %s\n", options->style->str);
                return -1;
            }
            styleCSS = katana_parse_in(fp);
            fclose(fp);
        } else {
            styleCSS = katana_parse(options->style->str, options->style->len, KatanaParserModeStylesheet);
        }
    }

    printf("Info: %s: %s => %s\n", __FUNCTION__, cstrbufGetStr(options->shpfile), cstrbufGetStr(options->outpng));

    // load shp file
    if (shapeFileInfoOpen(&shpInfo, "C:/WORKSPACE/vulkan-dev/Samples/shapetool/shps/area.shp") != 0) {
        katana_destroy_output(styleCSS);
        return -1;
    }

    // create cairo canvas
    if (cairoDrawCtxInit(&CDC, shpInfo.minBounds, shpInfo.maxBounds,
        options->width, options->height, options->dpi) == -1) {
        shapeFileInfoClose(&shpInfo);
        katana_destroy_output(styleCSS);
        return -1;
    }

    // draw shapes onto canvas
    shapeFileInfoDraw(&shpInfo, &CDC, styleCSS);

    katana_destroy_output(styleCSS);

    status = cairo_surface_write_to_png(CDC.surface, cstrbufGetStr(options->outpng));

    cairoDrawCtxFinal(&CDC);
    shapeFileInfoClose(&shpInfo);

    return status;
}