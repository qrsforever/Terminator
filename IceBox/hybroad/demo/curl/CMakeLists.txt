SET (CURL_TARGET_NAME test_curl)

LIST (APPEND CURL_SRC_LIST 
    test_curl.cpp
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PRO_THIIRDPART_PATH}/curl/include
    ${PLATFORM_SDK_PATH}/include
    )

LINK_DIRECTORIES(
    ${PLATFORM_SDK_PATH}/rootfs/lib
    ${PLATFORM_SDK_PATH}/rootfs/usr/lib
    ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
    )

ADD_EXECUTABLE (${CURL_TARGET_NAME} ${CURL_SRC_LIST})

TARGET_LINK_LIBRARIES (${CURL_TARGET_NAME} 
    curl
    ssl
    crypto
    )

#TARGET_LINK_LIBRARIES (${CARIO_TARGET_NAME} -Wl,--start-group xxxxxxxx.a -Wl,--end-group)
