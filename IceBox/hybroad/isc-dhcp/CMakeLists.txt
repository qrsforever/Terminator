#### dhcpcd ####

SET (DHCLIENT_TARGET_NAME dhclient)

ADD_DEFINITIONS (-DCLIENT_PATH="PATH=/root/:/sbin:/bin:/usr/bin:/home/${PLATFORM_HOME}/bin")
ADD_DEFINITIONS (-DLOCALSTATEDIR="/var")
ADD_DEFINITIONS (-D_PATH_RESCONF="/var")
ADD_DEFINITIONS (-DHAVE_CONFIG_H)
ADD_DEFINITIONS (-DMINIRES_LIB)
ADD_DEFINITIONS (-DHMAC_MD5)
ADD_DEFINITIONS (-DHYBROAD_USED)

INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/includes
    )

INCLUDE (${CMAKE_CURRENT_LIST_DIR}/common/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/omapip/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/minires/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/dst/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/client/CMakeLists.txt)

LINK_DIRECTORIES( 
    ${PLATFORM_SDK_PATH}/rootfs/lib
    ${PLATFORM_SDK_PATH}/rootfs/usr/lib 
    ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib
)

ADD_EXECUTABLE (${DHCLIENT_TARGET_NAME} ${DHCLIENT_SRC_LIST})

TARGET_LINK_LIBRARIES (${DHCLIENT_TARGET_NAME} pthread rt m gcc ssl crypto)

#TODO 
ADD_CUSTOM_COMMAND( TARGET ${DHCLIENT_TARGET_NAME} POST_BUILD 
    COMMAND cp -arpf ${EXECUTABLE_OUTPUT_PATH}/${DHCLIENT_TARGET_NAME} ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin
    COMMAND cp -arpf ${CMAKE_CURRENT_LIST_DIR}/client/dhclient.conf ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share
    COMMAND cp -arpf ${CMAKE_CURRENT_LIST_DIR}/client/scripts/dhclient.script ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
    COMMAND cp -arpf ${CMAKE_CURRENT_LIST_DIR}/client/scripts/dhclient.connect ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
    COMMAND cp -arpf ${CMAKE_CURRENT_LIST_DIR}/client/scripts/dhclient.disconnect ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
    COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/dhclient.script 
    COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/dhclient.connect 
    COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/dhclient.disconnect 
    )
