LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

INCLUDE (${CMAKE_CURRENT_LIST_DIR}/EasySetup/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/NTP/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Settings/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/StateNotify/CMakeLists.txt)
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/Thunder/CMakeLists.txt)

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/NetworkAssertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NetworkCard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NetworkInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NetworkManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WiredNetwork.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WirelessNetwork.cpp
    )

