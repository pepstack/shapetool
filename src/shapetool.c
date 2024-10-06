/**
 * @file shapetool.c
 * @author 350137278@qq.com
 * @brief A command line tool for manipulating ESRI shape (.shp) files
 * @version 0.0.1
 * @date 2024-10-03
 * 
 * @copyright Copyright (c) 2024, mapaware.top
 * 
 * @note
 *     https://github.com/pepstack/shapefile
 * 
 */
#include "shapetool-common.h"

shapetool_flags flags = {0};
shapetool_options options = {0};


static void onexit_cleanup(void)
{
    cstrbufFree(&options.shpfile);
    cstrbufFree(&options.outpng);
    cstrbufFree(&options.styleclass);
    cstrbufFree(&options.stylecss);

    printf("shapetool exit.\n");
}

typedef enum
{
    command_first = 0,
    command_drawshape = command_first,
    command_end
} shapetool_command;

static const char *commands[] = {
    "drawshape",
    "statshape",
    0
};


/**
 * @brief main
 *   程序主入口点
 * @param argc 
 * @param argv 
 * @return int
 * 
 *   $ shapetool drawshape --shpfile=/path/to/input.shp --outpng=/path/to/output.png
 */
int main(int argc, char * argv[])
{
    int errcode, opt, optindex, flag;

    // 从命令行解析命令名 cmdname:
    char cmdname[20] = {0};
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
            if (! strncmp(cmdname, commands[command], cmdnamelen)) {
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
            switch(flag) {
            case SHAPETOOL_OPT_SHPFILE:
                flags.shpfile = 1;
                options.shpfile = cstrbufDup(options.shpfile, optarg, cstrbuf_error_size_len);
                cstr_replace_chr(options.shpfile->str, '\\', '/');
                break;
            case SHAPETOOL_OPT_OUTPNG:
                flags.outpng = 1;
                options.outpng = cstrbufDup(options.outpng, optarg, cstrbuf_error_size_len);
                cstr_replace_chr(options.outpng->str, '\\', '/');
                break;
            case SHAPETOOL_OPT_WIDTH:
                flags.width = 1;
                options.width = atoi(optarg);
                break;
            case SHAPETOOL_OPT_HEIGHT:
                flags.height = 1;
                options.height = atoi(optarg);
                break;
            case SHAPETOOL_OPT_DPI:
                flags.dpi = 1;
                options.dpi = atoi(optarg);
                break;
            case SHAPETOOL_OPT_STYLECLASS:
                flags.styleclass = 1;
                options.styleclass = cstrbufDup(options.styleclass, optarg, cstrbuf_error_size_len);
                break;
            case SHAPETOOL_OPT_STYLECSS:
                flags.stylecss = 1;
                options.stylecss = cstrbufDup(options.stylecss, optarg, cstrbuf_error_size_len);
                cstr_replace_chr(options.stylecss->str, '\\', '/');
                break;
            }
            break;
        }
    }

    // default settings for view canvas
    if (! flags.width) {
        options.width = CAIRO_DRAW_WIDTH_DEFAULT;
    }
    if (! flags.height) {
        options.height = CAIRO_DRAW_HEIGHT_DEFAULT;
    }
    if (options.width < 10 || options.width > CAIRO_DRAW_WIDTH_MAX) {
        printf("Error: invalid map width=%.0f\n", options.width);
        exit(1);
    }
    if (options.height < 10 || options.height > CAIRO_DRAW_HEIGHT_MAX) {
        printf("Error: invalid map height=%.0f\n", options.height);
        exit(1);
    }

    // check and vlidate DPI
    if (! flags.dpi) {
        options.dpi = dpi_high_display;
    }
    if (options.dpi < dpi_draft_display) {
        options.dpi = dpi_draft_display;
    } else if (options.dpi > dpi_high_print) {
        options.dpi = dpi_high_print;
    }

    printf("Info: width=%.0f, height=%.0f, dpi=%d\n", options.width, options.height, options.dpi);

    if (! flags.stylecss) {
        // If both style css file not given:
        //   set 'a.shp' with default style css file: 'a.css'
        printf("%s\n", options.shpfile->str);
        options.stylecss = cstrbufDup(options.stylecss, options.shpfile->str,  options.shpfile->len);
        options.stylecss->str[options.stylecss->len - 3] = 'c';
        options.stylecss->str[options.stylecss->len - 2] = 's';
        options.stylecss->str[options.stylecss->len - 1] = 's';
        printf("Info: use default style css: %s\n", options.stylecss->str);
    }

    switch(command) {
    case command_drawshape:
        errcode = shpfile2png(&options);
        if (errcode) {
            exit(errcode);
        }
        break;
    default:
        break;
    }

    return 0;
}


static void print_usage()
{

}
