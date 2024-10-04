## shapetool

A command line tool for manipulating ESRI shape (.shp) files


### 项目依赖

- shapefile - ESRI shp file api

    $ cd shapetool/deps/

    $ git clone https://github.com/pepstack/shapefile

    $ cd shapefile

    $ make

- katana-parser - CSS parser lib

    $ git clone https://github.com/pepstack/katana-parser.git

    $ cd katana-parser

    $ ./autogen.sh

    // $ ./configure CFLAGS="-std=c99" --prefix=/path/to/install-dir
    $ ./configure CFLAGS="-std=c11"      // mingw: install to: $MSYS2_HOME/mingw64/bin/libkatana-1.dll

    $ make && make install

    $ pkg-config --cflags --libs katana  // print both

### Build shapetool

    $ cd shapetool/

    $ make
