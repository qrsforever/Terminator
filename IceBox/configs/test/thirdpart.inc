##all thirdpart libs configuration

libname:zlib
    depends=
    version=1.2.6

libname:jpeg
    depends=
    version=8d

libname:freetype
    depends=
    version=2.4.9

libname:sqlite
    depends=
    version=3.6.23.1

libname:libxml2
    depends=
    version=2.6.32

libname:libpng  
    depends=zlib
    version=1.5.10

libname:fontconfig
    depends=zlib freetype libxml2
    version=2.6.0

libname:pixman
    depends=libpng
    version=0.32.6

libname:cairo
    depends=fontconfig pixman
    version=1.10.0
