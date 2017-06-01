IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/local.rc)
    EXECUTE_PROCESS(
        COMMAND cp local.rc ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/local.rc)

IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/yxapi.conf)
    EXECUTE_PROCESS(
        COMMAND cp yxapi.conf ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/.yxapi.conf
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/yxapi.conf)

IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/libzebra.so)
    EXECUTE_PROCESS(
        COMMAND cp libzebra.so ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/libzebra.so)

IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/libnexus.so)
    EXECUTE_PROCESS(
        COMMAND cp libnexus.so ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/lib/
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/libnexus.so)

IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/fw_bcm4356a2_pcie_ag.bin)
    EXECUTE_PROCESS(
        COMMAND cp fw_bcm4356a2_pcie_ag.bin ${PLATFORM_SDK_PATH}/rootfs/home/${PLATFORM_HOME}/share/wlan
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
ENDIF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/fw_bcm4356a2_pcie_ag.bin)
