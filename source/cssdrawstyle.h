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
 * @date 2024-10-12 13:46:10
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


#ifdef  __cplusplus
}
#endif
#endif /* CSS_DRAW_STYLE_H__ */