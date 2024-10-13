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
 * Author: Light Zhang <mapaware.top>
 */
#ifndef SHAPETOOL_COMMON_H__
#define SHAPETOOL_COMMON_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include "shapetool-version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32API
# include <win32/getoptw.h>

# if !defined(__MINGW__)
    // link to pthread-w32 lib for MS Windows with MSVC
#pragma comment(lib, "pthreadVC2.lib")
# endif
#else
    // Linux: see Makefile
#   include <getopt.h>
#endif

#include <common/misc.h>

#include "cairodrawctx.h"


#ifdef __LINUX__
# define AAAA
#else
# define AAAA
#endif

#define SHAPETOOL_NAME_MAXLEN       30

#define SHAPETOOL_PATHLEN_INVALID  256


#define SHAPETOOL_OPT_DRAWSHP    1
#define SHAPETOOL_OPT_SHPFILE    2
#define SHAPETOOL_OPT_OUTPNG     3
#define SHAPETOOL_OPT_WIDTH      4      // width in dots
#define SHAPETOOL_OPT_HEIGHT     5      // height in dots
#define SHAPETOOL_OPT_DPI        6      // dots per inch

#define SHAPETOOL_OPT_STYLECLASS 7      // style class names
#define SHAPETOOL_OPT_STYLECSS   8      // style css file (/path/to/style.css)

#define FILE_URI_PREFIX  "file://"
#define FILE_URI_PREFIX_LEN      7      // strlen("file://")

#define PATH_IS_FILE_URI(path)      ((const char *) strstr((path), FILE_URI_PREFIX)) == ((const char*)(path))

// file:///path/to => /path/to
#define FILE_URI_PATH(szpath)         (&((szpath)[FILE_URI_PREFIX_LEN]))
#define CSTR_FILE_URI_PATH(cspath)    FILE_URI_PATH(CBSTR(cspath))


typedef enum {
    command_first = 0,
    command_drawshape = command_first,
    command_end
} shapetool_command;


static const char* commands[] = {
    "drawshape",
    "statshape",
    0
};


typedef struct {
    unsigned int shpfile : 1;
    unsigned int outpng : 1;
    unsigned int width : 1;
    unsigned int height : 1;
    unsigned int dpi : 1;
    unsigned int styleclass : 1;
    unsigned int stylecss : 1;
} shapetool_flags;


typedef struct {
    cstrbuf shpfile;
    cstrbuf outpng;

    cstrbuf styleclass;  // style class names
    cstrbuf stylecss;    // style css file (/path/to/style.css)

    float   width;      // width in dots
    float   height;     // height in dots
    int     dpi;
} shapetool_options;

extern int shpfile2png(shapetool_flags *flags, shapetool_options* options);

#ifdef    __cplusplus
}
#endif
#endif /* SHAPETOOL_COMMON_H__ */