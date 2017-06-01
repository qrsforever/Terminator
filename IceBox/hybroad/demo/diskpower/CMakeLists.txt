SET (DISKPOWER_TARGET_NAME test_diskpower)

INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    )

SET (ICEBOX_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../../icebox/src)

LIST (APPEND DISKPOWER_SRC_LIST 
    test_diskpower.cpp
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PLATFORM_SDK_PATH}/include
    )

LINK_DIRECTORIES(
    ${PLATFORM_SDK_PATH}/rootfs/lib
    ${PLATFORM_SDK_PATH}/rootfs/usr/lib
    ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
    )

ADD_EXECUTABLE (${DISKPOWER_TARGET_NAME} ${DISKPOWER_SRC_LIST})

TARGET_LINK_LIBRARIES (${DISKPOWER_TARGET_NAME} 
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
