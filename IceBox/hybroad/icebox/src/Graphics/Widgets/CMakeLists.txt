LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/Widget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AQIWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FanStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/HDiskStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SDCardStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/USBStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WifiStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WeatherWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UPSStatusWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DateWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TimeWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PromptWidget.cpp
    )
