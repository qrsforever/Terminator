INCLUDE (${CMAKE_CURRENT_LIST_DIR}/cairo/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/curl/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/freetype/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/json/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/pixman/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/sqlite/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/fontconfig/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/xmpp/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/ZenLib/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/MediaInfo/CMakeLists.txt)

LIST (APPEND LIBRARIES
    cairo
    freetype
    jpeg
    json
    pixman-1
    png
    sqlite3
    curl
    )

ADD_CUSTOM_TARGET(lib-all
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "all" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-zlib
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "zlib" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-libpng
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "libpng" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-jpeg
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "jpeg" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-freetype
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "freetype" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-libxml2
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "libxml2" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-fontconfig
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "fontconfig" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-pixman
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "pixman" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-cairo
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "cairo" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )

ADD_CUSTOM_TARGET(lib-sqlite
    COMMAND exec "${PRO_SCRIPT_PATH}/gen_thirdpart_libs.sh" "sqlite" "${PRO_CONFIG_PATH}" "${PRO_THIIRDPART_PATH}" "${PLATFORM_CROSS_COMPILER}"
    WORKING_DIRECTORY ${mycloud_BINARY_DIR}
    VERBATIM
    )
