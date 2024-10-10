## shapetool

https://github.com/pepstack/shapetool.git

A command line tool for manipulating ESRI shape (.shp) files.

一个命令行程序，用于处理 ESRI Shape (*.shp) 文件。

声明：本软件不对使用者做任何保证，由此带来的后果均与软件作者无关。你可以为任何目的使用本软件，前提是保留软件的版权声明。


Copyright (c) 2024 MapAware Inc. mapaware.top


### 编译 shapetool

#### 项目依赖目录

    shapetool/deps/

解压 shapetool/deps-YYYYMMDD.tar.gz 为 deps。必要时要编译依赖工程。

- deps/shapefile 最新代码下载地址:

     https://github.com/pepstack/shapefile


#### VisualStudio2022 编译
    
      shapetool/projects/msvc/shapetool/shapetool-vs2022.sln

#### VSCode 编译

      $ cd shapetool/
      $ code .

  按 [F5]

#### MinGW64 编译

      $ cd shapetool/
      $ make


#### Linux 平台上编译

      $ cd shapetool/
      $ make
