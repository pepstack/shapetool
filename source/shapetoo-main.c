/**
 * @file shapetoo-main.c
 *   A command line tool for manipulating ESRI shape (.shp) files
 *
 * @author mapaware@hotmail.com
 * @copyright © 2024-2030 mapaware.top All Rights Reserved.
 * @version 0.0.5
 *
 * @since 2024-10-03 00:05:20
 * @date 2024-10-14 01:07:40
 *
 * @note
 *     https://github.com/pepstack/shapefile
 */
#include "shapetool-common.h"

shapetool_flags flags = { 0 };
shapetool_options options = { 0 };


static void onexit_cleanup(void)
{
    cstrbufFree(&options.shpfile);
    cstrbufFree(&options.outpng);
    cstrbufFree(&options.styleclass);
    cstrbufFree(&options.stylecss);
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
 */
int main(int argc, char* argv[])
{
    WINDOWS_CRTDBG_ON

    int errcode, opt, optindex, flag, blen;

    // 从命令行解析命令名 cmdname:
    char cmdname[20] = { 0 };
    int cmdnamelen = 0;

    shapetool_command command = command_end;

    const struct option longopts[] = {
        {"help", no_argument, 0, 'h'}
        ,{"version", no_argument, 0, 'V'}
        ,{"shpfile", required_argument, &flag, SHAPETOOL_OPT_SHPFILE}
        ,{"outpng", required_argument, &flag, SHAPETOOL_OPT_OUTPNG}
        ,{"width", required_argument, &flag, SHAPETOOL_OPT_WIDTH}
        ,{"height", required_argument, &flag, SHAPETOOL_OPT_HEIGHT}
        ,{"dpi", required_argument, &flag, SHAPETOOL_OPT_DPI}
        ,{"style-class", required_argument, &flag, SHAPETOOL_OPT_STYLECLASS}
        ,{"style-css", required_argument, &flag, SHAPETOOL_OPT_STYLECSS}
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

    for (command = command_first; command != command_end; command++) {
        if (cmdnamelen == cstr_length(commands[command], sizeof(cmdname))) {
            if (!strncmp(cmdname, commands[command], cmdnamelen)) {
                break;
            }
        }
    }

    if (command == command_end) {
        printf("Error: command not found: %s\n", cmdname);
        exit(1);
    }

    printf("Info: execute command: %s\n", commands[command]);

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
            case SHAPETOOL_OPT_SHPFILE:
                blen = check_pathfile_arg(optarg, ".shp", 1);
                if (set_options_file(optarg, blen, &options.shpfile)) {
                    flags.shpfile = 1;
                }
                break;
            case SHAPETOOL_OPT_OUTPNG:
                blen = check_pathfile_arg(optarg, ".png", -1);
                if (set_options_file(optarg, blen, &options.outpng)) {
                    flags.outpng = 1;
                }
                break;
            case SHAPETOOL_OPT_WIDTH:
                options.width = (float) atoi(optarg);
                if (options.width < CAIRO_DRAW_WIDTH_MIN || options.width > CAIRO_DRAW_WIDTH_MAX) {
                    printf("Error: invalid map width=%.0f\n", options.width);
                    exit(1);
                }
                flags.width = 1;
                break;
            case SHAPETOOL_OPT_HEIGHT:
                options.height = (float) atoi(optarg);
                if (options.height < CAIRO_DRAW_HEIGHT_MIN || options.height > CAIRO_DRAW_HEIGHT_MAX) {
                    printf("Error: invalid map height=%.0f\n", options.height);
                    exit(1);
                }
                flags.height = 1;
                break;
            case SHAPETOOL_OPT_DPI:
                options.dpi = atoi(optarg);
                if (options.dpi < dpi_draft_display || options.dpi > dpi_high_print) {
                    printf("Error: invalid map dpi=%d\n", options.dpi);
                    exit(1);
                }
                flags.dpi = 1;
                break;
            case SHAPETOOL_OPT_STYLECLASS:
                flags.styleclass = 1;
                options.styleclass = cstrbufDup(options.styleclass, optarg, cstrbuf_error_size_len);
                break;
            case SHAPETOOL_OPT_STYLECSS:
                blen = cstr_length(optarg, CSS_KEYINDEX_INVALID_4096);
                if (blen == 0 || blen == CSS_KEYINDEX_INVALID_4096) {
                    printf("Error: invalid style css\n");
                    exit(1);
                }
                if (strchr(optarg, '{') && strchr(optarg, '}') && strchr(optarg, '{') != optarg) {
                    // is css string
                    options.stylecss = cstrbufDup(options.stylecss, optarg, cstrbuf_error_size_len);
                    flags.stylecss = 1;
                }
                else {
                    blen = check_pathfile_arg(optarg, ".css", 1);
                    if (set_options_file(optarg, blen, &options.stylecss)) {
                        flags.stylecss = 1;
                    }
                }
                break;
            }
            break;
        }
    }

    printf("Command: %s\n", commands[command]);

    if (command == command_drawshape) {
        if (! flags.shpfile) {
            printf("Error: no input shp file specified (use: --shpfile SHPFILE).\n");
            exit(1);
        }

        if (! flags.outpng) {
            printf("Error: no output png file specified (use: --outpng PNGFILE)\n");
            exit(1);
        }

        if (! flags.stylecss) {
            // If both stylecss not given:
            //   set 'a.shp' with default style css file: 'a.css'
            options.stylecss = cstrbufCat(options.stylecss, "%.*s.css", CBSTRLEN(options.shpfile) - 4, CBSTR(options.shpfile));
            printf("Info: use default style css: %s\n", options.stylecss->str);

            // check if default css file exists
            if (! pathfile_exists(CSTR_FILE_URI_PATH(options.stylecss))) {
                printf("Error: style css file not found: %.*s\n", CBSTRLEN(options.stylecss), CBSTR(options.stylecss));
                exit(1);
            }
            flags.stylecss = 1;
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

        errcode = shpfile2png(&flags, &options);
        if (errcode) {
            exit(errcode);
        }
    }

    // TODO: others

    // success exit
    return 0;
}

