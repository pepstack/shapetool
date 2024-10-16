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
 * @file cssdrawstyle.h
 * @brief
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.12
 *
 * @since 2024-10-03 00:05:20
 * @date 2024-10-16 22:07:01
 *
 * @note
 */
#ifndef CSS_DRAW_STYLE_H__
#define CSS_DRAW_STYLE_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include <common/cstrbuf.h>
#include <common/cssparse.h>


typedef struct {
    float red;
    float green;
    float blue;
} CssColorRGB;


typedef struct {
    float red;
    float green;
    float blue;
    float alpha;
} CssColorRGBA;


typedef enum {
    CSS_BORDER_NONE,
    CSS_BORDER_SOLID,
    CSS_BORDER_DASHED
} CssBorderStyle;


typedef enum {
    CSS_FILL_NONE,
    CSS_FILL_SOLID,
    CSS_FILL_DASHED
} CssFillStyle;


typedef struct
{
    // 多边形的外轮廓:
    int border_width;
    CssBorderStyle border_style;
    CssColorRGB border_color;

    // 多边形的内部填充:
    int fill_opacity;    // 0-100
    CssFillStyle fill_style;
    CssColorRGB fill_color;
} CssDrawStyle;


static CssKeyArray cssStyleLoadString(const char* cssarg, int csslen)
{
    CssString cssString = CssStringNew(cssarg, csslen);
    CssKeyArray keys = CssStringParse(cssString);
    if (!keys) {
        CssStringFree(cssString);
    }
    return keys;
}


static CssKeyArray cssStyleLoadFile(const char* csspathfile)
{
    FILE * cssfile = fopen(csspathfile, "r");
    if (cssfile) {
        CssString cssString = CssStringNewFromFile(cssfile);
        fclose(cssfile);

        if (!cssString) {
            printf("Error: CssStringNewFromFile() failed. cssfile=%s\n", csspathfile);
            exit(1);
        }

        CssKeyArray keys = CssStringParse(cssString);
        if (!keys) {
            printf("Error: CssStringParse() failed\n");
            CssStringFree(cssString);
            exit(1);
        }

        CssKeyArrayPrint(keys, stdout);

        // success
        return keys;
    }

    // error
    return 0;
}

#ifdef  __cplusplus
}
#endif
#endif /* CSS_DRAW_STYLE_H__ */