LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerBoot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerEasysetup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerRunning.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerDiskCopy.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerUpgrade.cpp
    )
