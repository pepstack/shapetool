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
    cstrbufFree(&options.style);

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
        ,{"style", required_argument, &flag, SHAPETOOL_OPT_STYLE}
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
            fprintf(stderr, "Error: option not defined\n");
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
                break;
            case SHAPETOOL_OPT_OUTPNG:
                flags.outpng = 1;
                options.outpng = cstrbufDup(options.outpng, optarg, cstrbuf_error_size_len);
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
            case SHAPETOOL_OPT_STYLE:
                flags.style = 1;
                options.style = cstrbufDup(options.style, optarg, cstrbuf_error_size_len);
                break;
            }
            break;
        }
    }

    // default settings for view canvas
    if (! flags.width) {
        // TODO: get screen width dots
        options.width = 3840;
    }
    if (! flags.height) {
        // TODO: get screen height dots
        options.height = 2160;
    }
    if (! flags.dpi) {
        // TODO: get screen device dpi
        options.dpi = 96;
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
