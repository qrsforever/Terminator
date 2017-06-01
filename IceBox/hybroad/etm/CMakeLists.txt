IF (NOT EXISTS ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder)
    EXECUTE_PROCESS(
        COMMAND mkdir -p ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/script/hubble_log.ini ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/script/log.ini ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/script/thunder_mounts.cfg ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/script/start_etm_monitor.sh ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/script/stop_etm_monitor.sh ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/bin/${ICEBOX_MODEL}/etm ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/bin/${ICEBOX_MODEL}/hubble ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/bin/${ICEBOX_MODEL}/vod_httpserver ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/
        COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/start_etm_monitor.sh
        COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/stop_etm_monitor.sh
        COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/etm
        COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/hubble
        COMMAND chmod 755 ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/bin/vod_httpserver
        )
ENDIF(NOT EXISTS ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/thunder)
