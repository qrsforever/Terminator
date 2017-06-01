LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/UpgradeAssertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UpgradeManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UpgradeDownload.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UpgradeConfig.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UpgradeBurning.cpp
    )
