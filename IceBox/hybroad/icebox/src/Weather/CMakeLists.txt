LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/WeatherAssertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WeatherManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WeatherParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WeatherRequest.cpp
    )
