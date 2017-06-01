#! /bin/bash

GEN_BUILD_INFO_DIR=$1

BUILD_INFO_FILE="$GEN_BUILD_INFO_DIR/build_info.h"
BUILD_DATE=`date`
BUILD_TIME=`date +%Y%m%d%H%M%S`
BUILD_USER=`whoami`

__get_git_version()
{
    export LANG="en_US.utf8"
    VERSION_NUMBER=`git rev-list HEAD | wc -l | awk '{print $1}'`
    VERSION_STRING="1.0.$VERSION_NUMBER $(git rev-list HEAD -n 1 | cut -c 1-7)"
}

__get_svn_version()
{
    export LANG="en_US.utf8"
    VERSION_NUMBER=`svn info | grep Revision | cut -d\: -f2 | sed 's/[[:space:]]//g'`
    echo "$VERSION_NUMBER"
    VERSION_STRING="1.0.$VERSION_NUMBER"
}

__get_svn_version

echo "#ifndef PROJECT_VERSION_H" > $BUILD_INFO_FILE
echo "#define PROJECT_VERSION_H" >> $BUILD_INFO_FILE
echo "" >> $BUILD_INFO_FILE
echo "#define BUILD_DATE \"$BUILD_DATE\"" >> $BUILD_INFO_FILE
echo "#define BUILD_TIME \"$BUILD_TIME\"" >> $BUILD_INFO_FILE
echo "#define BUILD_USER \"$BUILD_USER\"" >> $BUILD_INFO_FILE
echo "#define VERSION_NUMBER $VERSION_NUMBER" >> $BUILD_INFO_FILE
echo "#define VERSION_STRING \"$VERSION_STRING\"" >> $BUILD_INFO_FILE
echo "" >> $BUILD_INFO_FILE
echo "#endif" >> $BUILD_INFO_FILE
