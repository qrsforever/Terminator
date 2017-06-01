SET (THUMB_TARGET_NAME test_thumbnail)

INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    )

SET (ICEBOX_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../../icebox/src)

LIST (APPEND THUMB_SRC_LIST 
    test_thumbnail.cpp
    ${ICEBOX_SRC_DIR}/Graphics/Thumbnail/Thumbnail.cpp
    ${ICEBOX_SRC_DIR}/Utils/Buffer.cpp
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}
    ${ICEBOX_SRC_DIR}/Graphics/Thumbnail/
    ${ICEBOX_SRC_DIR}/Utils/
    ${PLATFORM_SDK_PATH}/include
    )

LINK_DIRECTORIES(
    ${PLATFORM_SDK_PATH}/rootfs/lib
    ${PLATFORM_SDK_PATH}/rootfs/usr/lib
    ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
    )

ADD_EXECUTABLE (${THUMB_TARGET_NAME} ${THUMB_SRC_LIST})

TARGET_LINK_LIBRARIES (${THUMB_TARGET_NAME} 
    cairo
    jpeg
    pixman-1
    png
    freetype
    bwl
    zebra
    nexus
    )

#TARGET_LINK_LIBRARIES (${CARIO_TARGET_NAME} -Wl,--start-group xxxxxxxx.a -Wl,--end-group)
