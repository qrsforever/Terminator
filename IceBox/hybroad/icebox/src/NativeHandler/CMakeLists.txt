LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerAssertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerCustomer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NativeHandlerPublic.cpp
    )

INCLUDE (${CMAKE_CURRENT_LIST_DIR}/State/CMakeLists.txt)
