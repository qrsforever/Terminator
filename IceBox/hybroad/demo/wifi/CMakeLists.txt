SET (CARIO_TARGET_NAME test_wifi)

INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    )

LIST (APPEND CARIO_SRC_LIST 
    test_wifi.cpp
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PLATFORM_SDK_PATH}/include
    ${PLATFORM_SDK_PATH}/include/sdk
    ${PLATFORM_SDK_PATH}/include/sdk/utility
    )

LINK_DIRECTORIES(
    ${PLATFORM_SDK_PATH}/rootfs/lib
    ${PLATFORM_SDK_PATH}/rootfs/usr/lib
    ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
    )

ADD_EXECUTABLE (${CARIO_TARGET_NAME} ${CARIO_SRC_LIST})

TARGET_LINK_LIBRARIES (${CARIO_TARGET_NAME} 
    cairo
    freetype
    jpeg
    pixman-1
    png
    bwl
    zebra
    nexus
    )

#TARGET_LINK_LIBRARIES (${CARIO_TARGET_NAME} -Wl,--start-group xxxxxxxx.a -Wl,--end-group)
