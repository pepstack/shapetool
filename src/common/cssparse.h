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
 * @file cssparse.h
 * @author 350137278@qq.com
 * @brief A simple css file parser
 *
 * @version    0.1.0
 * @create     2024-10-07
 * @date       2024-10-07
 * 
 * file: <test.css>
 
    .polygon {
        border-width: 3px;
        border-style: solid;
        border-color: #FFFF00;
        fill-opacity: 1;
        fill-style: solid;
        fill-color: #00FFFF;
    }

    * {
        border-width: 4px;
        border-style: dash;
        border-color: #00FFF00;
        fill-opacity: 0;
        fill-style: solid;
        fill-color: #0011FF;
    }

    #123 {
        border-width: 5px;
        border-style: none;
        border-color: #AAFF00;
        fill-opacity: 10;
        fill-style: solid;
        fill-color: #00CCFF;
    }

 * Show how to parse css file:

void parse_print_cssfile(const char *cssfile)
{
    FILE * fp = fopen(cssfile, "r");
    if (fp) {
        CssString cssString = 0;
        int numKeys = 0;
        void * cssOutKeys = 0;

        // 第一次测试空间以分配内存
        cssString = CssParseFile(fp, 0, &numKeys);

        if (cssString && numKeys < 0) {
            // cssString: 非 0 表示从文件读取正确
            // numKeys: < 0 表示期望分配内存=(-numKeys)*sizeof(struct CssKeyField)
            numKeys = -numKeys;
            cssOutKeys = malloc(sizeof(struct CssKeyField) * numKeys);

            if (CssParseString(cssString, (struct CssKeyField *) cssOutKeys, &numKeys) && numKeys > 0) {
                // 使用 cssOutKeys
                CssPrintKeys(cssString, (struct CssKeyField *) cssOutKeys, numKeys);
            }

            // 用户必须释放自己分配的内存
            free(cssOutKeys);
        }

        // 必须释放 CssParseFile 返回的字符串的内存
        CssStringFree(cssString);

        // 用户必须关闭自己打开的文件句柄
        fclose(fp);
    }
}

enjoy it!
 */
#ifndef CSS_PARSE_H__
#define CSS_PARSE_H__

#if defined(__cplusplus)
extern "C"
{
#endif

typedef char * CssString;
typedef struct CssKeyField * CssKeyArray;

typedef enum {
    css_type_null = 0,
    css_type_class = 1,    // .class
    css_type_id = 2,       // #id
    css_type_asterisk = 3, // *
    css_type_key = 6,
    css_type_value = 7
} CssKeyType;


extern CssKeyArray CssKeyArrayNew(int num);

extern void CssKeyArrayFree(CssKeyArray keys);

extern char * CssParseString(char *cssString, CssKeyArray outKeys, int *numKeys);

extern CssString CssParseFile(FILE *cssFile, CssKeyArray outKeys, int *numKeys);

// CssString 类型释放器, 仅仅用于释放 CssParseFile() 返回的对象
extern void CssStringFree(CssString cssString);

// 方法展示了如何使用 cssparse 解析输出 css
///extern int CssKeysFindFirst(const CssString cssString, const CssKeyArray cssKeys, int numKeys);

extern void CssPrintKeys(const CssString cssString, const CssKeyArray cssKeys, int numKeys);

#ifdef __cplusplus
}
#endif
#endif /* CSS_PARSE_H__ */
