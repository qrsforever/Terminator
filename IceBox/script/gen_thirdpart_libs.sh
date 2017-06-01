#! /bin/bash

KEY_WORD=thirdpart

BUILD_DIR=$KEY_WORD
DST_LIB=$1
COMMON_FILE=$2/$KEY_WORD.inc
LIBS_SRC_DIR=$3
CROSS_COMPILER=$4

GREP_PREFIX="libname:"

export CFLAGS="-fPIC -DPIC -O2"
export LDFLAGS=""
export AS=${CROSS_COMPILER}as
export LD=${CROSS_COMPILER}ld
export CC=${CROSS_COMPILER}gcc
export CXX=${CROSS_COMPILER}g++
export AR=${CROSS_COMPILER}ar
export NM=${CROSS_COMPILER}nm
export STRIP=${CROSS_COMPILER}strip
export OBJCOPY=${CROSS_COMPILER}objcopy
export OBJDUMP=${CROSS_COMPILER}objdump
export RANLIB=${CROSS_COMPILER}ranlib

__RUN()
{
    echo $1
    $1
}

__travel()
{
    if [ "x$1" == "x" ]; then
        return
    fi

    line=`grep -i -A2 "$GREP_PREFIX[ \t]*$1" $COMMON_FILE | grep -i "depends="`
    line=`echo $line | sed 's/^[ \t]*//g'`
    line=`echo $line | sed 's/[ \t]*$//g'`
    line=`echo $line | egrep -v "(^$)|(^#)"`

    deps=(${line#depends=})
    if [ "x$deps" != "x" ]; then
        for item in ${deps[@]}
        do
            echo "$1 depend: $item"
            if [ -e .${item}_ver.inc ]; then
                continue
            else
                __travel $item
            fi
        done
    fi

    line=`grep -i -A2 "$GREP_PREFIX[ \t]*$1" $COMMON_FILE | grep -i "version="`
    line=`echo $line | sed 's/^[ \t]*//g'`
    line=`echo $line | sed 's/[ \t]*$//g'`
    line=`echo $line | egrep -v "(^$)|(^#)"`

    if [ "x$line" == "x" ]; then
        return 
    fi

    ver=(${line#version=})
    if [ ! -e $1 ]; then
        __RUN "make -f $LIBS_SRC_DIR/$1/Makefile-$ver clean"
        __RUN "tar zxf $LIBS_SRC_DIR/$1/$1-$ver.tar.gz -C ."
    fi
    __RUN "make -f $LIBS_SRC_DIR/$1/Makefile-$ver"
}

__GlobalInclude()
{
    path=$1
    file=$path"/.global.inc"
    echo "###global include" > $file
    echo "OUT_DIR:=$path/outdir" >> $file
    echo "HOST:=${CROSS_COMPILER%-}" >> $file
    echo "PLATFORM:=${ICEBOX_MODEL}" >> $file

    #echo "SQUASH_OUTPUT:=>/dev/null 2>&1" >> $file
    echo "SQUASH_OUTPUT:=" >> $file
}

__main()
{
    if [ ! -e $BUILD_DIR ]; then
        mkdir -p $BUILD_DIR
    fi
    cd $BUILD_DIR

    __GlobalInclude `pwd`

    if [ $DST_LIB == "all" ]; then
        cat $COMMON_FILE | egrep -v "(^$)|(^#)" | grep "$GREP_PREFIX" | while read line
        do
            lib=`echo $line | cut -d: -f2`
            __travel $lib
        done
    else 
        __travel $DST_LIB
    fi
}

__main
