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
 * @file shapetool-main.c
 * @brief A command line tool for manipulating ESRI shape (.shp) files.
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.13
 *
 * @since 2024-10-03 00:05:20
 * @date 2024-10-17 03:01:17
 *
 * @note
 */
#include "shapetool-common.h"

shapetool_flags flags = { 0 };
shapetool_options options = { 0 };


static void onexit_cleanup(void)
{
    CssKeyArrayFree(options.cssStyleKeys);

    cstrbufFree(&options.layerscfg);
    cstrbufFree(&options.mapid);
    cstrbufFree(&options.shpfile);
    cstrbufFree(&options.outpng);
    cstrbufFree(&options.styleclass);
}


static int check_pathfile_arg(char *filearg, const char *fileext, int existflag)
{
    int blen = cstr_length(filearg, SHAPETOOL_PATHLEN_INVALID);
    if (blen == SHAPETOOL_PATHLEN_INVALID) {
        printf("Error: invalid path file: %s\n", optarg);
        exit(1);
    }
    if (! cstr_endwith(filearg, blen, fileext, (int)strlen(fileext))) {
        printf("Error: file type mismatch(%s): %s\n", fileext, optarg);
        exit(1);
    }

    int found = pathfile_exists(optarg);
    if (existflag > 0) {
        // 1: file must be exist
        if (! found) {
            printf("Error: file not found: %s\n", optarg);
            exit(1);
        }
    } else if (existflag < 0) {
        // -1: file must be NOT exist
        if (found) {
            printf("Error: file exists: %s\n", optarg);
            exit(1);
        }
    }

    return blen;
}


static cstrbuf set_options_file(char* optargstr, int argchlen, cstrbuf* pathfile)
{
    cstrbuf cstr = 0;
    if (argchlen > 0) {
        if (cstr_startwith(optargstr, argchlen, FILE_URI_PREFIX, FILE_URI_PREFIX_LEN)) {
            cstr = cstrbufDup(cstr, optargstr, argchlen);
        }
        else {
            cstr = cstrbufCat(cstr, "%.*s%.*s", FILE_URI_PREFIX_LEN, FILE_URI_PREFIX, argchlen, optargstr);
        }

        cstr_replace_chr(CBSTR(cstr), '\\', '/');
        *pathfile = cstr;
    }
    return cstr;
}


static void print_usage()
{
    // TODO:
}

/**
 * @brief main
 *   程序主入口点
 * @param argc
 * @param argv
 * @return int
 *
 *   $ shapetool drawshape --shpfile=/path/to/input.shp --outpng=/path/to/output.png
 *
 * DEBUG:
 *   $ shapetool drawshape --shpfile ../../../shps/area.shp --outpng ../../../output/area2.png
 *
 *   $ shapetool drawshape --shpfile ../../../shps/area.shp --outpng ../../../output/area2.png --stylecss ".polygon { border: 3 solid #000FFF; fill: 1 solid #CFF000}"
 *
 *   $ shapetool drawlayers --layerscfg map-layers.cfg --mapid default --outpng ../../../output/map-default.png
 */
int main(int argc, char* argv[])
{
    WINDOWS_CRTDBG_ON

    int opt, optindex, flag, blen;

    // 从命令行解析命令名 cmdname:
    char cmdname[20] = { 0 };
    int cmdnamelen = 0;

    shapetool_command command = command_end_npos;

    const struct option longopts[] = {
        {"help", no_argument, 0, 'h'}
        ,{"version", no_argument, 0, 'V'}
        ,{"shpfile", required_argument, &flag, optarg_shpfile}
        ,{"layerscfg", required_argument, &flag, optarg_layerscfg}
        ,{"mapid", required_argument, &flag, optarg_mapid}
        ,{"outpng", required_argument, &flag, optarg_outpng}
        ,{"width", required_argument, &flag, optarg_width}
        ,{"height", required_argument, &flag, optarg_height}
        ,{"dpi", required_argument, &flag, optarg_dpi}
        ,{"styleclass", required_argument, &flag, optarg_styleclass}
        ,{"stylecss", required_argument, &flag, optarg_stylecss}
        ,{0, 0, 0, 0}
    };

    if (argc == 1) {
        printf("Error: command not specified\n");
        exit(1);
    }

    cmdnamelen = snprintf(cmdname, sizeof(cmdname), "%s", argv[1]);
    if (cmdnamelen < 4 || cmdnamelen == sizeof(cmdname)) {
        printf("Error: bad command: %s\n", argv[1]);
        exit(1);
    }

    for (command = command_first_pos; command != command_end_npos; command++) {
        if (cmdnamelen == cstr_length(commands[command], sizeof(cmdname))) {
            if (!strncmp(cmdname, commands[command], cmdnamelen)) {
                break;
            }
        }
    }

    if (command == command_end_npos) {
        printf("Error: command not found: %s\n", cmdname);
        exit(1);
    }

    printf("Info: Exec command: %s\n", commands[command]);

    atexit(onexit_cleanup);

    while ((opt = getopt_long_only(argc, argv, "hV", longopts, &optindex)) != -1) {
        switch (opt) {
        case '?':
            printf("Error: option not defined\n");
            exit(1);
            break;
        case 'h':
            print_usage();
            break;
        case 'V':
            printf("shapetool-%s, Build: %s %s\nCopyright (c) 2024, mapaware.top\n", SHAPETOOL_VERSION_STRING, __DATE__, __TIME__);
            break;
        case 0:
            switch (flag) {
            case optarg_layerscfg:
                blen = check_pathfile_arg(optarg, ".cfg", 1);
                if (set_options_file(optarg, blen, &options.layerscfg)) {
                    flags.layerscfg = 1;
                }
                break;
            case optarg_mapid:
                options.mapid = cstrbufDup(options.mapid, optarg, cstrbuf_error_size_len);
                break;
            case optarg_shpfile:
                blen = check_pathfile_arg(optarg, ".shp", 1);
                if (set_options_file(optarg, blen, &options.shpfile)) {
                    flags.shpfile = 1;
                }
                break;
            case optarg_outpng:
                blen = check_pathfile_arg(optarg, ".png", -1);
                if (set_options_file(optarg, blen, &options.outpng)) {
                    flags.outpng = 1;
                }
                break;
            case optarg_width:
                options.width = (float) atoi(optarg);
                if (options.width < CAIRO_DRAW_WIDTH_MIN || options.width > CAIRO_DRAW_WIDTH_MAX) {
                    printf("Error: invalid map width=%.0f\n", options.width);
                    exit(1);
                }
                flags.width = 1;
                break;
            case optarg_height:
                options.height = (float) atoi(optarg);
                if (options.height < CAIRO_DRAW_HEIGHT_MIN || options.height > CAIRO_DRAW_HEIGHT_MAX) {
                    printf("Error: invalid map height=%.0f\n", options.height);
                    exit(1);
                }
                flags.height = 1;
                break;
            case optarg_dpi:
                options.dpi = atoi(optarg);
                if (options.dpi < dpi_draft_display || options.dpi > dpi_high_print) {
                    printf("Error: invalid map dpi=%d\n", options.dpi);
                    exit(1);
                }
                flags.dpi = 1;
                break;
            case optarg_styleclass:
                flags.styleclass = 1;
                options.styleclass = cstrbufDup(options.styleclass, optarg, cstrbuf_error_size_len);
                break;
            case optarg_stylecss:
                blen = cstr_length(optarg, CSS_KEYINDEX_INVALID_4096);
                if (blen == 0 || blen == CSS_KEYINDEX_INVALID_4096) {
                    printf("Error: invalid style css\n");
                    exit(1);
                }
                cstrbuf cssPathfile = 0;
                if (strchr(optarg, '{') && strchr(optarg, '}') && strchr(optarg, '{') != optarg) {
                    // css string
                    options.cssStyleKeys = cssStyleLoadString(optarg, blen);
                }
                else if (set_options_file(optarg, blen, &cssPathfile)) {
                    options.cssStyleKeys = cssStyleLoadFile(CSTR_FILE_URI_PATH(cssPathfile));
                }
                cstrbufFree(&cssPathfile);
                if (options.cssStyleKeys) {
                    flags.style = 1;
                }
                break;
            }
            break;
        }
    }

    // exec command
    if (command == command_drawshape) {
        if (! flags.shpfile) {
            printf("Error: no input shp file specified (use: --shpfile SHPFILE).\n");
            exit(1);
        }

        if (! flags.outpng) {
            printf("Error: no output png file specified (use: --outpng PNGFILE)\n");
            exit(1);
        }

        if (! flags.style) {
            // If both stylecss not given:
            //   set 'a.shp' with default style css file: 'a.css'
            cstrbuf cssPathfile = cstrbufCat(0, "%.*s.css", CBSTRLEN(options.shpfile) - 4, CBSTR(options.shpfile));
            printf("Info: use default style css: %s\n", CBSTR(cssPathfile));

            // check if default css file exists
            if (! pathfile_exists(CSTR_FILE_URI_PATH(cssPathfile))) {
                printf("Error: style css file not found: %.*s\n", CBSTRLEN(cssPathfile), CBSTR(cssPathfile));
                exit(1);
            }
            options.cssStyleKeys = cssStyleLoadFile(CSTR_FILE_URI_PATH(cssPathfile));
            cstrbufFree(&cssPathfile);

            if (options.cssStyleKeys) {
                flags.style = 1;
            }
        }

        // default settings for view canvas
        if (!flags.width) {
            options.width = CAIRO_DRAW_WIDTH_DEFAULT;
        }
        if (!flags.height) {
            options.height = CAIRO_DRAW_HEIGHT_DEFAULT;
        }

        if (!flags.dpi) {
            // set default dpi if no dpi specified
            options.dpi = dpi_high_display;
        }

        printf("Info: shpfile2png: %s => %s\n", CBSTR(options.shpfile), CBSTR(options.outpng));
        printf("      png: width=%.0f, height=%.0f, dpi=%d\n", options.width, options.height, options.dpi);

        shpfile2png(&flags, &options);
    }
    else if (command == command_drawlayers) {
        if (!flags.layerscfg) {
            printf("Error: no layers config file specified (use: --layerscfg CFGFILE).\n");
            exit(1);
        }

        if (!flags.outpng) {
            printf("Error: no output png file specified (use: --outpng PNGFILE)\n");
            exit(1);
        }
        if (!options.mapid) {
            printf("Warn: no mapid specified (use: --mapid MAPID). so we use [map:default])\n");
            options.mapid = cstrbufDup(options.mapid, "default", 7);
        }

        maplayers2png(&flags, &options);
    }

    // TODO: others

    // success exit
    return 0;
}

