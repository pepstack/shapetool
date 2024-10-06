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
 * @date:   2024-10-06
 */
#ifndef CSS_DRAW_STYLE_H__
#define CSS_DRAW_STYLE_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include <common/cstrbuf.h>

// css parser
// https://blog.csdn.net/weixin_41036447/article/details/113731281
#include <katana.h>


#define CSS_CLASS_MAXNUM    10


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


typedef struct {
    int numClasses;
    char *classNames[CSS_CLASS_MAXNUM];
    int nameLens[CSS_CLASS_MAXNUM];
} CssClassGroup;


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
} CssPolygonStyle;


// https://blog.csdn.net/weixin_41036447/article/details/113731281
//
static void CssParseRuleStyle(KatanaStyleRule * styleRule, CssClassGroup * classGroup)
{
    int i;
    const KatanaArray *selectors = styleRule->selectors;

    for (i = 0; i < selectors->length; i++) {
        KatanaSelector *sel = (KatanaSelector *) selectors->data[i];

        switch (sel->match) {
        case KatanaSelectorMatchTag:
            printf("Processing selector is: %s\n", (sel->tag) ? sel->tag->local : "UNNAMED");
            //apply_rule(sr->declarations);
            break;
        case KatanaSelectorMatchId:
            break;
        case KatanaSelectorMatchClass:
            printf("Processing class selector: %s\n", (sel->data) ? sel->data->value : "UNNAMED");
            break;
        case KatanaSelectorMatchPseudoClass: // E.g. a:link
            break;
        case KatanaSelectorMatchPseudoElement:
            break;
        case KatanaSelectorMatchPagePseudoClass:
            break;
        case KatanaSelectorMatchAttributeExact:
            break;
        case KatanaSelectorMatchAttributeSet:
            break;
        case KatanaSelectorMatchAttributeList:
            break;
        case KatanaSelectorMatchAttributeHyphen:
            break;
        case KatanaSelectorMatchAttributeContain:
            break;
        case KatanaSelectorMatchAttributeBegin:
            break;
        case KatanaSelectorMatchAttributeEnd:
            break;
        case KatanaSelectorMatchUnknown:
            break;
        }
    }
}


static int CssParseStyleFile(const char *cssFile, CssClassGroup * classGroup)
{
    KatanaOutput *cssOutput = 0;

    FILE *fp = fopen(cssFile, "r");
    if (fp) {
        cssOutput = katana_parse_in(fp);
        fclose(fp);
    }

    if (! cssOutput || ! cssOutput->stylesheet) {
        printf("Error: katana_parse_in() failed for css file: %s\n", cssFile);
        return -1;
    }

    KatanaStylesheet *sheet = cssOutput->stylesheet;
    for (int i = 0; i < sheet->rules.length; ++i) {
        KatanaRule *rule = (KatanaRule *) sheet->rules.data[i];
        if (! rule) {
            printf("Warn: rules is empty\n");
            continue;
        }
        if (rule->type == KatanaRuleStyle) {
            CssParseRuleStyle((KatanaStyleRule*) rule, classGroup);            
        } else {
            printf("Warn: Rule type not supported: %d\n", rule->type);
            continue;
        }
    }

    return 0;
}


#ifdef    __cplusplus
}
#endif
#endif /* CSS_DRAW_STYLE_H__ */