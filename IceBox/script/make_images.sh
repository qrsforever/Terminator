#! /bin/bash
ROOTFS_DIR=$1
CROSS_COMPILER=$2
PROJECT_TOP_DIR=$3

STRIP_CMD=${CROSS_COMPILER}strip
CURR_DIR=`pwd`
IMAGES_DIR=$CURR_DIR/images
SDK_BIN=$CURR_DIR/sdk/bin

BUILD_DATE=`date`
BUILD_USER=`whoami`
BUILD_VERSION=`cat $CURR_DIR/build_info.h | grep VERSION_NUMBER | cut -d\  -f3`

__copy()
{
    if [ ! -d "$ROOTFS_DIR/home/hybroad/bin" ]; then
        echo "NO $ROOTFS_DIR/home/hybroad/bin dir."
        exit 1
    fi
    cp -af $CURR_DIR/bin/* $ROOTFS_DIR/home/hybroad/bin
}

__strip_bin()
{
    if [ -z $ROOTFS_DIR ]; then
        exit 1
    fi
    echo "-- strip files in path: $ROOTFS_DIR"
    for ff in `find "$ROOTFS_DIR" -type f`; do
        ck=`echo "$ff" | grep "$ROOTFS_DIR"`
        if [ -z "$ck" ]; then
            continue
        fi
        #driver.ko can not strip.
        fl=`file $ff | grep 'not stripped' | grep -v \.ko`
        if [ -n "$fl" ]; then
            fl=`echo $fl | cut -d ':' -f 1`
            $STRIP_CMD $fl
        fi
    done
}

__make_initrd_fs()
{
    echo "===>make initrd rootfs"
    INITRD_O=$IMAGES_DIR/.initrd 
    if [ ! -d $INITRD_O ]; then 
        mkdir -p $INITRD_O
    fi
    tar zxf $PROJECT_TOP_DIR/sdk/$ICEBOX_MODEL/$MAKEUP_INITRD_GTAR -C $INITRD_O

    #TODO 7252
    $PROJECT_TOP_DIR/script/gen_vmlinuz_7252.sh $INITRD_O $ROOTFS_DIR $CROSS_COMPILER
    
    if [ ! -f $INITRD_O/vmlinuz ]; then
        echo "===> no generate $INITRD_O/vmlinuz"
        exit 1
    fi

    VMZ=$INITRD_O/vmlinuz 
    $SDK_BIN/mkup.elf -model $ICEBOX_MODEL -pcb 1 -force -chipset $ICEBOX_CHIPSET -date '$BUILD_DATE' -author '$BUILD_USER' -major 1 -minor 0 -svn 34 -target kernel0 $VMZ > $IMAGES_DIR/HY7252_ICEBOX_1.0.$BUILD_VERSION.bin

    echo "IceBox.Version=1.0.$BUILD_VERSION" > $IMAGES_DIR/config.ini
    echo "IceBox.Software=HY7252_ICEBOX_1.0.$BUILD_VERSION.bin" >> $IMAGES_DIR/config.ini
}

__make_ext4_fs()
{
    echo "===>make ext4 rootfs"

    if [ ! -d $IMAGES_DIR ]; then 
        mkdir -p $IMAGES_DIR 
    fi
    rm $IMAGES_DIR/.rootfs_* -rf
    cd $ROOTFS_DIR && tar -czf $IMAGES_DIR/.rootfs_$BUILD_VERSION.tar.gz *
    $SDK_BIN/mkup.elf -model $ICEBOX_MODEL -pcb 1 -force -chipset $ICEBOX_CHIPSET -date '$BUILD_DATE' -author '$BUILD_USER' -major 1 -minor 0 -svn 34 -filesystem 2 -target root0 -compressor 1 $IMAGES_DIR/.rootfs_$BUILD_VERSION.tar.gz > $IMAGES_DIR/HY7252_ICEBOX_1.0.$BUILD_VERSION.bin
}

__copy
__strip_bin

if [ -z "$MAKEUP_INITRD_GTAR" ]; then
    __make_ext4_fs
else 
    __make_initrd_fs
fi
