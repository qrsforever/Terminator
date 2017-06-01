IF (NOT EXISTS ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/desktop)
    EXECUTE_PROCESS(
        COMMAND mkdir -p ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/desktop
        COMMAND cp -arf ${CMAKE_CURRENT_LIST_DIR}/desktop ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/
        )
ENDIF(NOT EXISTS ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/desktop)
