LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

INCLUDE (${CMAKE_CURRENT_LIST_DIR}/DataStream/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Devices/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Disk/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/FileManager/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Graphics/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Log/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/MediaInfo/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Message/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/NativeHandler/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Network/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Platform/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Setting/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Upgrade/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Utils/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Views/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Weather/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/XmppShell/CMakeLists.txt)

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/Assertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IBoxMain.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MainThread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PromptMessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SystemKeyEvent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SystemManager.cpp
    )
