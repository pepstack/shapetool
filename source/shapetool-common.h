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
 * @file shapetool-common.h
 * @brief A command line tool for manipulating ESRI shape (.shp) files.
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.14
 *
 * @since 2024-10-16 22:13:24
 * @date 2024-10-17 01:22:30
 *
 * @note
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
#include <common/cssparse.h>

#include "cairodrawctx.h"


#ifdef __LINUX__
# define AAAA
#else
# define AAAA
#endif

#define SHAPETOOL_RES_SOK      0
#define SHAPETOOL_RES_ERR    (-1)


#define SHAPETOOL_NAMELEN_MAX       30
#define SHAPETOOL_PATHLEN_INVALID  256


#define FILE_URI_PREFIX  "file://"
#define FILE_URI_PREFIX_LEN      7      // strlen("file://")

#define PATH_IS_FILE_URI(path)      ((const char *) strstr((path), FILE_URI_PREFIX)) == ((const char*)(path))

// file:///path/to => /path/to
#define FILE_URI_PATH(szpath)         (&((szpath)[FILE_URI_PREFIX_LEN]))
#define CSTR_FILE_URI_PATH(cspath)    FILE_URI_PATH(CBSTR(cspath))


static const char* commands[] = {
    "drawshape",
    "drawlayers",
    0
};

typedef enum {
    command_first_pos = 0,
    command_drawshape = command_first_pos,
    command_drawlayers,
    command_end_npos
} shapetool_command;


typedef enum {
    optarg_layerscfg = 1,
    optarg_mapid,
    optarg_shpfile,
    optarg_outpng,
    optarg_width,          // width in dots
    optarg_height,         // height in dots
    optarg_dpi,            // dots per inch
    optarg_styleclass,     // style class names
    optarg_stylecss        // style css file (/path/to/style.css)
} shapetool_optarg;


typedef struct {
    unsigned int layerscfg : 1;
    unsigned int shpfile : 1;
    unsigned int outpng : 1;
    unsigned int width : 1;
    unsigned int height : 1;
    unsigned int dpi : 1;
    unsigned int styleclass : 1;
    unsigned int style : 1;
} shapetool_flags;


typedef struct {
    cstrbuf layerscfg;
    cstrbuf mapid;
    cstrbuf shpfile;
    cstrbuf outpng;

    cstrbuf styleclass;  // style class names
    CssKeyArray cssStyleKeys; // css parsed keys

    float   width;      // width in dots
    float   height;     // height in dots
    int     dpi;
} shapetool_options;


int shpfile2png(shapetool_flags *flags, shapetool_options* options);

int maplayers2png(shapetool_flags* flags, shapetool_options* options);

#ifdef    __cplusplus
}
#endif
#endif /* SHAPETOOL_COMMON_H__ */